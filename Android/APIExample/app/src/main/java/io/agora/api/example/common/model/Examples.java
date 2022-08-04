package io.agora.api.example.common.model;

import androidx.annotation.NonNull;

import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import io.agora.api.example.annotation.Example;

public class Examples {
    public static final String BASIC = "BASIC";
    public static final String ADVANCED = "ADVANCED";

    public static final Map<String, List<Example>> ITEM_MAP = new HashMap<>();

    public static void addItem(@NonNull Example item) {
        String group = item.group();
        List<Example> list = ITEM_MAP.get(group);
        if (list == null) {
            list = new ArrayList<>();
            ITEM_MAP.put(group, list);
        }
        list.add(item);
    }

    public static void sortItem() {
        for (Map.Entry<String, List<Example>> entry : ITEM_MAP.entrySet()) {
            List<Example> exampleList = ITEM_MAP.get(entry.getKey());
            Collections.sort(exampleList, new Comparator<Example>() {
                @Override
                public int compare(Example o1, Example o2) {
                    return o1.index() - o2.index();
                }
            });
        }
    }
}
