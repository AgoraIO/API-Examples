package io.agora.api.example.examples.advanced;

import io.agora.api.example.R;
import io.agora.api.example.annotation.Example;
import io.agora.api.example.common.BaseFragment;

import static io.agora.api.example.common.model.Examples.ADVANCED;

@Example(
        index = 17,
        group = ADVANCED,
        name = R.string.item_geofencing,
        actionId = R.id.action_mainFragment_to_GeoFencing,
        tipsId = R.string.geofencing
)
public class GeoFencing extends BaseFragment {
}
