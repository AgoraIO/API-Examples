package io.agora.api.example.compose.utils;

import android.content.Context;
import android.content.res.AssetManager;
import android.text.TextUtils;

import org.jetbrains.annotations.NotNull;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;

/**
 * The type File utils.
 */
public final class FileUtils {

    private FileUtils() {

    }

    /**
     * The constant SEPARATOR.
     */
    public static final String SEPARATOR = File.separator;

    /**
     * Copy files from assets.
     *
     * @param context     the context
     * @param assetsPath  the assets path
     * @param storagePath the storage path
     */
    public static void copyFilesFromAssets(Context context, String assetsPath, String storagePath) {
        String temp = "";

        if (TextUtils.isEmpty(storagePath)) {
            return;
        } else if (storagePath.endsWith(SEPARATOR)) {
            storagePath = storagePath.substring(0, storagePath.length() - 1);
        }

        if (TextUtils.isEmpty(assetsPath) || assetsPath.equals(SEPARATOR)) {
            assetsPath = "";
        } else if (assetsPath.endsWith(SEPARATOR)) {
            assetsPath = assetsPath.substring(0, assetsPath.length() - 1);
        }

        AssetManager assetManager = context.getAssets();
        try {
            File file = new File(storagePath);
            if (!file.exists()) { //If the folder does not exist, create a new folder
                file.mkdirs();
            }

            // Get all file and directory names under the assets directory
            String[] fileNames = assetManager.list(assetsPath);
            if (fileNames.length > 0) { //If it is a directory apk
                for (String fileName : fileNames) {
                    if (!TextUtils.isEmpty(assetsPath)) {
                        temp = assetsPath + SEPARATOR + fileName; //Complete the assets resource path
                    }

                    String[] childFileNames = assetManager.list(temp);
                    if (!TextUtils.isEmpty(temp) && childFileNames.length > 0) { //Determine if it is a file or folder: if it is a folder
                        copyFilesFromAssets(context, temp, storagePath + SEPARATOR + fileName);
                    } else { //If it is a file
                        InputStream inputStream = assetManager.open(temp);
                        readInputStream(storagePath + SEPARATOR + fileName, inputStream);
                    }
                }
            } else { //If it is a file doc_test.txt or apk/app_test.apk
                InputStream inputStream = assetManager.open(assetsPath);
                if (assetsPath.contains(SEPARATOR)) { //apk/app_test.apk
                    assetsPath = assetsPath.substring(assetsPath.lastIndexOf(SEPARATOR), assetsPath.length());
                }
                readInputStream(storagePath + SEPARATOR + assetsPath, inputStream);
            }
        } catch (IOException e) {
            e.printStackTrace();
        }

    }

    /**
     * Read data from input stream and write to output stream
     *
     * @param storagePath Target file path
     * @param inputStream Input stream
     */
    public static void readInputStream(String storagePath, InputStream inputStream) {
        File file = new File(storagePath);
        try {
            if (!file.exists()) {
                // 1. Create channel object
                FileOutputStream fos = new FileOutputStream(file);
                // 2. Define storage space
                byte[] buffer = new byte[inputStream.available()];
                // 3. Start reading file
                int lenght = 0;
                while ((lenght = inputStream.read(buffer)) != -1) { // Loop to read buffer bytes from input stream
                    // Write the data in the Buffer to the outputStream object
                    fos.write(buffer, 0, lenght);
                }
                fos.flush(); // Flush buffer
                // 4. Close streams
                fos.close();
                inputStream.close();
            }
        } catch (FileNotFoundException e) {
            e.printStackTrace();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    @NotNull
    public static String getAssetsString(@NotNull Context context, @NotNull String path) {
        StringBuilder sb = new StringBuilder();
        InputStreamReader isr = null;
        BufferedReader br = null;

        try {
            isr = new InputStreamReader(context.getResources().getAssets().open(path));
            br = new BufferedReader(isr);
            String line = null;
            while ((line = br.readLine()) != null){
                sb.append(line).append("\n");
            }
        } catch (IOException e) {
            throw new RuntimeException(e);
        } finally {
            if (isr != null) {
                try {
                    isr.close();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
            if (br != null) {
                try {
                    br.close();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        }
        return sb.toString();
    }
}







