package com.bytedance.labcv.common.utils;


/** {zh} 
 * 通用工具
 */
/** {en} 
 * General Tools
 */

public class CommonUtils {
    private static long lastclicked = 0;
    private static final long INTERVAL = 500;

    /** {zh} 
     * 判断快速点击
     * @return
     */
    /** {en} 
     * Judge quick click
     * @return
     */

     public static boolean isFastClick(){
         if (System.currentTimeMillis() - lastclicked < INTERVAL) {
             lastclicked = System.currentTimeMillis();
             return true;
         }
         lastclicked = System.currentTimeMillis();
         return false;
     }





}
