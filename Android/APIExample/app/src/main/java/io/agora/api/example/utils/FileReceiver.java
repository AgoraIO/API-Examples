package io.agora.api.example.utils;

import android.util.Log;

import org.json.JSONObject;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.UnsupportedEncodingException;
import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;
import java.util.Date;

import io.agora.api.example.examples.advanced.FileTransfer;

public class FileReceiver {
    private static final String TAG = FileReceiver.class.getSimpleName();

    final int SEND_FILE_PACKAGE_LENGTH = 1024;  // 每个文件数据包长度，小于这个长度说明为特殊协议包
    private String file_name = "";
    private String file_md5 = "";
    private int file_size = 0;
    private int file_progress = 0;
    private long recv_file_size = 0;
    private int transfer_kbps = 0;
    private int file_total_num = 0;
    private int file_fail_num = 0;
    private MessageDigest md;
    private FileOutputStream recvFile = null;
    Long last_print_ts = 0L;
    Long curr_print_ts = 0L;
    Long last_recv_size = 0L;
    Long curr_recv_size = 0L;
    int transfer_state = 0;

    private void closeFile() {
        if (file_size > 0) {
            if (recvFile != null) {
                try {
                    recvFile.flush();
                    recvFile.close();
                } catch (IOException e) {
                    Log.i(TAG, "receive file close failed.");
                }
                recvFile = null;
            }
            file_total_num += 1;
            byte[] digest = md.digest();
            String result = toHexString(digest);
            if (!result.equals(file_md5)) {
                file_fail_num += 1;
                Log.e(TAG, String.format("MD5 cannot compare %s with %s", file_md5, result));
            }
            file_size = 0;
            recv_file_size = 0L;
            file_md5 = "";
            file_name = "";
        }
    }

    private int recvFileStart(byte[] data) {
        try {
            String msg = new String(data, "UTF-8");
            JSONObject parser = new JSONObject(msg);
            if (!parser.has("name") || !parser.has("size") || !parser.has("md5")) {
                Log.i(TAG, "unexpected start package, ignore it.");
                return -1;
            }
            closeFile();
            file_name = parser.getString("name");
            file_size = parser.getInt("size");
            file_md5 = parser.getString("md5");
        } catch (Exception e) {
            Log.i(TAG, String.format("unexpected start package, ignore it."));
            return -1;
        }
        // create file
        try {
            recvFile = new FileOutputStream("/sdcard/" + file_name);
        } catch (Exception e) {
            Log.i(TAG, String.format("cannot create file: %s", file_name));
        }
        // reset md5 calculator
        try {
            md = MessageDigest.getInstance("MD5");
            md.reset();
        } catch (Exception e) {
            Log.i(TAG, String.format("cannot create MD5 object."));
        }
        recv_file_size = 0L;
        last_recv_size = 0L;
        curr_recv_size = 0L;
        return 0;
    }

    private int recvFileEnd(byte[] data) {
        try {
            String msg = new String(data, "UTF-8");
            JSONObject parser = new JSONObject(msg);
            if (!parser.has("eof")) {
                Log.i(TAG, "unexpected end package, ignore it.");
                return -1;
            }
        } catch (Exception e) {
            Log.i(TAG, String.format("unexpected start package, ignore it."));
            return -1;
        }
        closeFile();
        return 0;
    }

    public void updateRecvProgress() {
        if (file_size > 0) {
            file_progress = (int) (recv_file_size * 100 / file_size);
            Long diff_time = 0L;
            Long diff_size = 0L;

            Date dt= new Date();
            Long curr_print_ts= dt.getTime();
            diff_time = curr_print_ts - last_print_ts;
            last_print_ts = curr_print_ts;
            curr_recv_size = recv_file_size;
            diff_size = curr_recv_size - last_recv_size;
            last_recv_size = curr_recv_size;
            transfer_kbps = (int) (diff_size * 8.0 / diff_time);
        } else {
            file_progress = 0;
        }
    }

    public void injectRdtData(byte[] data) {
        if (SEND_FILE_PACKAGE_LENGTH != data.length) {
            if (recvFileStart(data) == 0) {
                return;
            } else if (recvFileEnd(data) == 0) {
                return;
            }
        }
        recv_file_size += data.length;
        md.update(data, 0, data.length);

        try {
            if (recvFile != null) {
                recvFile.write(data);
            }
        } catch (IOException e) {
            Log.i(TAG, "write file failed.");
        }
    }

    private static String toHexString(byte[] digest) {
        StringBuilder sb = new StringBuilder();
        String hexStr;
        for (byte b : digest) {
            hexStr = Integer.toHexString(b & 0xFF);     //& 0xFF处理负数
            if (hexStr.length() == 1) {                   //长度等于1，前面进行补0，保证最后的字符串长度为32
                hexStr = "0" + hexStr;
            }
            sb.append(hexStr);
        }
        return sb.toString();
    }

    public void setTransferState(int state) {transfer_state = state;}

    public String getFileName() { return file_name;}

    public int getFileProgress() { return file_progress;}

    public int getTransferKbps() { return  transfer_kbps;}

    public int getFileTotalNum() { return file_total_num;}

    public int getFileFailNum() { return file_fail_num;}

    public String getState() {
        switch (transfer_state) {
            case 0:
                return "RDT_STATE_CLOSED";
            case 1:
                return "RDT_STATE_OPENED";
            case 2:
                return "RDT_STATE_BLOCKED";
            case 3:
                return "RDT_STATE_PENDING";
            case 4:
                return "RDT_STATE_BROKEN";
            default:
                return "RDT_STATE_UNKNOWN";
        }
    }

    public void reset() {
        file_name = "";
        file_progress = 0;
        transfer_kbps = 0;
    }
}
