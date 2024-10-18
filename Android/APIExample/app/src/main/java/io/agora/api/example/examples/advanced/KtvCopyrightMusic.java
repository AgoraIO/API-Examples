package io.agora.api.example.examples.advanced;

import static io.agora.api.example.common.model.Examples.ADVANCED;

import java.util.Locale;

import io.agora.api.example.R;
import io.agora.api.example.annotation.Example;
import io.agora.api.example.common.BaseBrowserFragment;

/**
 * The type Ktv copyright music.
 */
@Example(
        index = 24,
        group = ADVANCED,
        name = R.string.item_ktv_copyright_music,
        actionId = R.id.action_mainFragment_to_ktv_copyright_music,
        tipsId = R.string.ktv_copyright_music
)
public class KtvCopyrightMusic extends BaseBrowserFragment {

    @Override
    protected String getBrowserUrl() {
        if (getResources().getConfiguration().locale.getLanguage() == Locale.CHINESE.getLanguage()) {
            return "https://doc.shengwang.cn/doc/online-ktv/android/ktv-scenario/landing-page";
        }
        return "https://docs.agora.io/en/interactive-live-streaming/overview/product-overview?platform=android";
    }

}
