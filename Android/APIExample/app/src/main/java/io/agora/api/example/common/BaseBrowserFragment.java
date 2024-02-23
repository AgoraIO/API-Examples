package io.agora.api.example.common;

import android.content.Intent;
import android.graphics.Bitmap;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewGroup;
import android.view.WindowManager;
import android.webkit.ConsoleMessage;
import android.webkit.JsResult;
import android.webkit.WebChromeClient;
import android.webkit.WebResourceError;
import android.webkit.WebResourceRequest;
import android.webkit.WebSettings;
import android.webkit.WebView;
import android.webkit.WebViewClient;

import androidx.activity.OnBackPressedCallback;
import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.appcompat.app.AlertDialog;
import androidx.core.view.MenuProvider;
import androidx.navigation.NavController;
import androidx.navigation.Navigation;

import java.lang.reflect.Field;
import java.lang.reflect.Method;
import java.util.Locale;

import io.agora.api.example.BuildConfig;
import io.agora.api.example.R;
import io.agora.api.example.databinding.FragmentBaseBrowserBinding;

/**
 * The type Base browser fragment.
 */
public abstract class BaseBrowserFragment extends BaseFragment {

    private FragmentBaseBrowserBinding mBinding;
    private WebView mWebView;

    @Override
    public final View onCreateView(@NonNull LayoutInflater inflater, @Nullable ViewGroup container, @Nullable Bundle savedInstanceState) {
        mBinding = FragmentBaseBrowserBinding.inflate(inflater, container, false);
        return mBinding.getRoot();
    }

    @Override
    public final void onViewCreated(@NonNull View view, @Nullable Bundle savedInstanceState) {
        super.onViewCreated(view, savedInstanceState);
        initWebview();
        initMunes();
        mWebView.loadUrl(getBrowserUrl());
    }

    @Override
    public void onPause() {
        super.onPause();
        mWebView.onPause();
        mWebView.getSettings().setJavaScriptEnabled(false);
    }

    @Override
    public void onResume() {
        super.onResume();
        mWebView.onResume();
        mWebView.getSettings().setJavaScriptEnabled(true);
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        mWebView.destroy();
        releaseWebView();
    }

    private void initWebview() {
        mWebView = new WebView(requireActivity());
        mBinding.webviewContainer.addView(mWebView);

        mWebView.setFocusable(true);
        mWebView.setFocusableInTouchMode(true);
        mWebView.setSaveEnabled(true);

        // 硬件加速
        requireActivity().getWindow().setFlags(
                WindowManager.LayoutParams.FLAG_HARDWARE_ACCELERATED,
                WindowManager.LayoutParams.FLAG_HARDWARE_ACCELERATED);

        // WebView 配置
        WebSettings webSettings = mWebView.getSettings();

        // 后退
        requireActivity().getOnBackPressedDispatcher().addCallback(getViewLifecycleOwner(), new OnBackPressedCallback(true) {
            @Override
            public void handleOnBackPressed() {
                if (mWebView.canGoBack()) {
                    mWebView.goBack();
                } else {
                    NavController navController = Navigation.findNavController(requireView());
                    navController.navigateUp();
                }
            }
        });

        // 缓存相关
        // mWebView.clearCache(true); // 清除缓存
        // mWebView.clearHistory(); // 清除历史
        // mWebView.clearFormData(); // 清除表单数据
        // 缓存模式
        // LOAD_DEFAULT:
        // 默认，根据 cache -control 决定是否从网络上取数据
        // LOAD_NORMAL:
        // API level 17 中已经废弃, 从API level 11 开始作用同 LOAD_DEFAULT 模式
        // LOAD_CACHE_ELSE_NETWORK:
        // 只要本地有，无论是否过期，或者 no -cache，都使用缓存中的数据
        // LOAD_NO_CACHE:
        // 不使用缓存，只从网络获取数据
        // LOAD_CACHE_ONLY:
        // 不使用网络，只读取本地缓存数据
        webSettings.setCacheMode(WebSettings.LOAD_NO_CACHE); // 设置缓存模式


        // js 相关
        webSettings.setJavaScriptEnabled(true); // 支持 js。如果碰到后台无法释放 js 导致耗电，应在 onStop 和 onResume 里分别设成 false 和 true
        webSettings.setJavaScriptCanOpenWindowsAutomatically(true);
        webSettings.setDomStorageEnabled(true);
        // mWebView.addJavascriptInterface(new WebAppInterface(this), "android"); // js 接口

        try {
            // webSettings.setPluginsEnabled(true); // 支持插件
            Method setPluginsEnabled = webSettings.getClass().getMethod("setPluginsEnabled", Boolean.class);
            setPluginsEnabled.setAccessible(true);
            setPluginsEnabled.invoke(webSettings, true);
        } catch (Exception e) {
            // do nothing
        }


        // 设置自适应屏幕，两者合用
        webSettings.setUseWideViewPort(true); // 将图片调整到适合 WebView 的大小
        webSettings.setLoadWithOverviewMode(true); // 缩放至屏幕的大小

        // 缩放操作
        webSettings.setSupportZoom(true); // 支持缩放，默认为 true
        webSettings.setBuiltInZoomControls(true); // 设置内置的缩放控件，若为 false，则该 WebView 不可缩放
        webSettings.setDisplayZoomControls(false); // 隐藏原生的缩放控件

        // 支持同时打开https和http
        webSettings.setMixedContentMode(WebSettings.MIXED_CONTENT_ALWAYS_ALLOW);

        mWebView.setWebChromeClient(new WebChromeClient() {

            @Override
            public boolean onConsoleMessage(ConsoleMessage consoleMessage) {
                Log.d("BrowserFragment", consoleMessage.messageLevel() + "--" + consoleMessage.message() + ":" + consoleMessage.lineNumber());
                return super.onConsoleMessage(consoleMessage);
            }

            @Override
            public void onShowCustomView(View view, CustomViewCallback callback) {
                super.onShowCustomView(view, callback);
            }

            @Override
            public boolean onJsAlert(WebView view, String url, String message, JsResult result) {
                return super.onJsAlert(view, url, message, result);
            }
        });
        mWebView.setWebViewClient(new WebViewClient() {

            @Override
            public boolean shouldOverrideUrlLoading(WebView view, WebResourceRequest request) {
                if (!request.getUrl().toString().equals(getBrowserUrl())) {
                    openWithDefaultBrowser(request.getUrl().toString());
                    return true;
                }
                return false;
            }

            @Override
            public void onPageStarted(WebView view, String url, Bitmap favicon) {
                super.onPageStarted(view, url, favicon);
                mBinding.loading.setVisibility(View.VISIBLE);
            }

            @Override
            public void onPageFinished(WebView view, String url) {
                super.onPageFinished(view, url);
                mBinding.loading.setVisibility(View.GONE);
            }

            @Override
            public void onReceivedError(WebView view, WebResourceRequest request, WebResourceError error) {
                super.onReceivedError(view, request, error);
                if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
                    new AlertDialog.Builder(view.getContext())
                            .setTitle("Error")
                            .setMessage(String.format(Locale.US, "%s\n\ncode:%d", error.getDescription().toString(), error.getErrorCode()))
                            .setPositiveButton(R.string.refresh, (dialog, which) -> {
                                mWebView.reload();
                            })
                            .show();
                }
            }
        });

        mWebView.setWebContentsDebuggingEnabled(true);
    }

    private void releaseWebView() {
        try {
            Field sConfigCallback = Class.forName("android.webkit.BrowserFrame").getDeclaredField("sConfigCallback");
            if (sConfigCallback != null) {
                sConfigCallback.setAccessible(true);
                sConfigCallback.set(null, null);
            }
        } catch (NoSuchFieldException e) {
            if (BuildConfig.DEBUG) {
                e.printStackTrace();
            }
        } catch (ClassNotFoundException e) {
            if (BuildConfig.DEBUG) {
                e.printStackTrace();
            }
        } catch (IllegalAccessException e) {
            if (BuildConfig.DEBUG) {
                e.printStackTrace();
            }
        }
    }

    private void initMunes() {
        requireActivity().addMenuProvider(new MenuProvider() {
            @Override
            public void onCreateMenu(@NonNull Menu menu, @NonNull MenuInflater menuInflater) {
                menuInflater.inflate(R.menu.menu_browser, menu);
            }

            @Override
            public boolean onMenuItemSelected(@NonNull MenuItem item) {
                if (item.getItemId() == R.id.open_in_browser) {
                    openWithDefaultBrowser(getBrowserUrl());
                    return true;
                } else if (item.getItemId() == R.id.refresh) {
                    mWebView.reload();
                    return true;
                }
                return false;
            }
        }, getViewLifecycleOwner());
    }

    private void openWithDefaultBrowser(String url) {
        Uri uri = Uri.parse(url);
        Intent intent = new Intent(Intent.ACTION_VIEW, uri);
        startActivity(intent);
    }

    /**
     * Gets browser url.
     *
     * @return the browser url
     */
    protected abstract String getBrowserUrl();

}
