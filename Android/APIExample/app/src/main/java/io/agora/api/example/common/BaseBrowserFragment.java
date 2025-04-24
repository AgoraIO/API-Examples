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

        // Hardware acceleration
        requireActivity().getWindow().setFlags(
                WindowManager.LayoutParams.FLAG_HARDWARE_ACCELERATED,
                WindowManager.LayoutParams.FLAG_HARDWARE_ACCELERATED);

        // WebView configuration
        WebSettings webSettings = mWebView.getSettings();

        // Back button handling
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

        // Cache related settings
        // mWebView.clearCache(true); // Clear cache
        // mWebView.clearHistory(); // Clear history
        // mWebView.clearFormData(); // Clear form data
        // Cache mode
        // LOAD_DEFAULT:
        // Default, decides whether to fetch from network based on cache-control
        // LOAD_NORMAL:
        // Deprecated in API level 17, from API level 11 it functions same as LOAD_DEFAULT
        // LOAD_CACHE_ELSE_NETWORK:
        // Use cached data if available, regardless of expiration or no-cache
        // LOAD_NO_CACHE:
        // Do not use cache, fetch data from network only
        // LOAD_CACHE_ONLY:
        // Do not use network, read only from local cache
        webSettings.setCacheMode(WebSettings.LOAD_NO_CACHE); // Set cache mode


        // js related
        webSettings.setJavaScriptEnabled(true); // Support js. If js cannot be released after background, it should be set to false in onStop and true in onResume
        webSettings.setJavaScriptCanOpenWindowsAutomatically(true);
        webSettings.setDomStorageEnabled(true);
        // mWebView.addJavascriptInterface(new WebAppInterface(this), "android"); // js interface

        try {
            // webSettings.setPluginsEnabled(true); // Support plugins
            Method setPluginsEnabled = webSettings.getClass().getMethod("setPluginsEnabled", Boolean.class);
            setPluginsEnabled.setAccessible(true);
            setPluginsEnabled.invoke(webSettings, true);
        } catch (Exception e) {
            // do nothing
        }


        // Set adaptive screen, both are used
        webSettings.setUseWideViewPort(true); // Adjust image to fit WebView size
        webSettings.setLoadWithOverviewMode(true); // Scale to screen size

        // Scaling operation
        webSettings.setSupportZoom(true); // Support zoom, default is true
        webSettings.setBuiltInZoomControls(true); // Set built-in zoom control, if false, this WebView cannot be zoomed
        webSettings.setDisplayZoomControls(false); // Hide native zoom control

        // Support opening https and http at the same time
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
                new AlertDialog.Builder(view.getContext())
                        .setTitle("Error")
                        .setMessage(String.format(Locale.US, "%s\n\ncode:%d", error.getDescription().toString(), error.getErrorCode()))
                        .setPositiveButton(R.string.refresh, (dialog, which) -> {
                            mWebView.reload();
                        })
                        .show();
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
