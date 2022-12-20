package com.bytedance.labcv.common.utils;

import java.lang.reflect.Field;

/**
 * Created on 2021/7/16 16:29
 */
public class ReflectUtils {

    public static void setObjectValue(Class<?> clz, Object obj, String name, Object value) throws IllegalAccessException {
        Field[] fields = clz.getDeclaredFields();
        for (Field field : fields) {
            if (field.getName().equals(name)) {
                field.setAccessible(true);
                field.set(obj, value);
            }
        }
    }

    public static Object getObjectValue(Class<?> clz, Object obj, String name) throws IllegalAccessException {
        Field[] fields = clz.getDeclaredFields();
        for (Field field : fields) {
            if (field.getName().equals(name)) {
                field.setAccessible(true);
                return field.get(obj);
            }
        }
        return null;
    }

    public static void setIntValue(Class<?> clz, Object obj, String name, int value) throws IllegalAccessException {
        Field[] fields = clz.getDeclaredFields();
        for (Field field : fields) {
            if (field.getName().equals(name)) {
                field.setAccessible(true);
                field.setInt(obj, value);
            }
        }
    }

    public static int getIntValue(Class<?> clz, Object obj, String name) throws IllegalAccessException {
        Field[] fields = clz.getDeclaredFields();
        for (Field field : fields) {
            if (field.getName().equals(name)) {
                field.setAccessible(true);
                return field.getInt(obj);
            }
        }
        return 0;
    }
}
