package io.agora.api.example.annotation;

import java.lang.annotation.ElementType;
import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;
import java.lang.annotation.Target;

@Target({ElementType.TYPE})
@Retention(RetentionPolicy.RUNTIME)
public @interface Example {
    /**
     * @return group name
     */
    String group() default "";

    /**
     * @return example name
     */
    String name() default "";

    /**
     * @return action ID
     */
    int actionId();
}
