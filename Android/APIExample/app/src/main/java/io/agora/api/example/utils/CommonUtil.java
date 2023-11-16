package io.agora.api.example.utils;

import android.app.Activity;
import android.content.Context;
import android.view.inputmethod.InputMethodManager;
import android.widget.EditText;

/**
 * The type Common util.
 *
 * @author cjw
 */
public final class CommonUtil {

    private CommonUtil() {

    }

    /**
     * Hide input board.
     *
     * @param activity the activity
     * @param editText the edit text
     */
    public static void hideInputBoard(Activity activity, EditText editText) {
        InputMethodManager imm = (InputMethodManager) activity.getSystemService(Context.INPUT_METHOD_SERVICE);
        imm.hideSoftInputFromWindow(editText.getWindowToken(), 0);
    }
}
