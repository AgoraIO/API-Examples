package io.agora.extension;

import android.content.Context;
import android.os.AsyncTask;
import android.util.Log;

public class UtilsAsyncTask extends AsyncTask {
    public interface OnUtilsAsyncTaskEvents {
        void onPreExecute();
        void onPostExecute();
    }

    private Context context = null;
    private OnUtilsAsyncTaskEvents events = null;

    public UtilsAsyncTask(Context context, OnUtilsAsyncTaskEvents events) {
        this.context = context;
        this.events = events;
    }

    @Override
    protected Object doInBackground(Object[] objects) {
        Log.d("Agora_zt", " doInBackground");
        ExtensionManager.copyResource(context);
        return null;
    }

    @Override
    protected void onPreExecute() {
        super.onPreExecute();
        events.onPreExecute();
    }

    @Override
    protected void onPostExecute(Object o) {
        super.onPostExecute(o);
        events.onPostExecute();
    }
}
