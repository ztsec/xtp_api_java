package com.zts.xtp.common.jni;

import java.io.File;
import java.util.HashMap;
import java.util.Map;

public class JNILoadLibrary {
    public static boolean glogHasInited = false;
    public static void loadLibrary() {
        loadLibrary(null);
    }
    public static void loadLibrary(String libraryPath) {
        if(libraryPath==null) libraryPath = "";
		String osLibPath = System.getProperty("user.dir");
        libraryPath = "/usr/local/lib:"+osLibPath+":c:/windows/system32";//libraryPath+"cpp/src:
        String os = System.getProperty("os.name").toLowerCase();
        boolean osIsLinux = (os.indexOf("linux") != -1);
        boolean osIsWindows = false;
        boolean osIsMacOsX = false;
        //首先加载工程目录下lib/xx系统/下的native lib，再加载用户配置的如cpp编译目录下的smart编译生成的lib以及系统公共目录如/usr/local/lib下的，生产上一般只有/usr/local/lib，找不到的不加载
        //如果一个lib出现在多个目录下，按照上述优先级加载，加载过的不再加载
        if(osIsLinux){
            libraryPath = "cpp/lib/linux:"+libraryPath; //加载工程目录下lib/linux下的xtp的so
        }else{
            osIsWindows = (os.indexOf("windows") != -1);
            if(osIsWindows){
                String arch = System.getProperty("os.arch");
                if("x86".equals(arch))
                    libraryPath = "cpp/lib/win32/dll:cpp/buildcpp/win32:"+libraryPath; //加载工程目录下lib/win32下的xtp的dll
                else
                    libraryPath = "cpp/lib/win64/dll:cpp/buildcpp/win64:"+libraryPath; //加载工程目录下lib/win64下的xtp的dll
            }else{
                osIsMacOsX = "mac os x".equals(os);
                libraryPath = "cpp/lib/macosx:"+libraryPath; //加载工程目录下lib/linux下的xtp的dylib
            }
        }
        //		System.out.println("====================="+libraryPath);
//        addLibraryDir(libraryPath);//linux没有起作用


        final boolean osIsWindows_final = osIsWindows;
        final boolean osIsMacOsX_final = osIsMacOsX;
        String[] libraryPathArray = libraryPath.split(":");
        Map<String,String> nativeLibHasLoaded = new HashMap<String,String>();
        for(int i=0;i<libraryPathArray.length;i++){
            String dirPath = libraryPathArray[i];
            File dir = new File(dirPath);
            File[] defiles = dir.listFiles(pathName -> {
                String name = pathName.getName();
                boolean isLib = false;
                if(osIsLinux)
                    isLib =  (name.startsWith("libtradeplugin") || name.startsWith("libquoteplugin") ||
                            name.startsWith("libxtptraderapi_xalgo") || name.startsWith("libxtpquoteapi") || name.startsWith("libsodium") || name.startsWith("libglog"))
                            && name.endsWith(".so");
                if(osIsMacOsX_final)
                    isLib =  (name.startsWith("libtradeplugin") || name.startsWith("libquoteplugin") ||
                            name.startsWith("libxtptraderapi_xalgo") || name.startsWith("libxtpquoteapi") || name.startsWith("libglog") )
                            && (name.endsWith(".dylib") || name.endsWith(".jnilib"));
                if(osIsWindows_final)
		  				isLib =  (name.startsWith("quoteplugin") || name.startsWith("tradeplugin") ||
                            name.startsWith("xtptraderapi_xalgo") || name.startsWith("xtpquoteapi")  || name.startsWith("glog") )
                            && name.endsWith(".dll");
                return isLib;
            });
            if (defiles == null || defiles.length == 0) {
                continue;
            }
            for (File defile : defiles) {
                String fileName=defile.getName();
                String prefix=fileName.substring(fileName.lastIndexOf("."));
                int num=prefix.length();
                String filerNameWithoutExt=fileName.substring(0, fileName.length()-num);
                if(nativeLibHasLoaded.containsKey(filerNameWithoutExt))
                    continue;
                System.load(defile.getAbsolutePath());
                System.out.println("load native lib:"+defile.getAbsolutePath());
                nativeLibHasLoaded.put(filerNameWithoutExt, "1");
            }

        }


        //如果用遍历定义目录方式没找到（如c:\windows\system32在一些高版本的windows上java用listFiles访问不到，可能需要管理员权限），因此在系统目录用LoadLibrary方式尝试再加载一遍，兜底
        if(!nativeLibHasLoaded.containsKey("glog") && !nativeLibHasLoaded.containsKey("libglog"))
            try{
                System.loadLibrary("glog");
                System.out.println("load native lib from Sys Path: "+"glog");
                nativeLibHasLoaded.put("glog", "1");
            }catch (Exception e){
                System.err.println("Fail to load native lib from Sys Path: "+"glog");
            }
        if(!nativeLibHasLoaded.containsKey("xtpquoteapi") && !nativeLibHasLoaded.containsKey("libxtpquoteapi"))
            try{
                System.loadLibrary("xtpquoteapi");
                System.out.println("load native lib from Sys Path: "+"xtpquoteapi");
                nativeLibHasLoaded.put("xtpquoteapi", "1");
            }catch (Exception e){
                System.err.println("Fail to load native lib from Sys Path: "+"xtpquoteapi");
            }
        if(!nativeLibHasLoaded.containsKey("xtptraderapi_xalgo") && !nativeLibHasLoaded.containsKey("libxtptraderapi_xalgo"))
            try{
                System.loadLibrary("xtptraderapi_xalgo");
                System.out.println("load native lib from Sys Path: "+"xtptraderapi_xalgo");
                nativeLibHasLoaded.put("xtptraderapi_xalgo", "1");
            }catch (Exception e){
                System.err.println("Fail to load native lib from Sys Path: "+"xtptraderapi_xalgo");
            }
        if(!nativeLibHasLoaded.containsKey("tradeplugin") && !nativeLibHasLoaded.containsKey("libtradeplugin"))
            try{
                System.loadLibrary("tradeplugin");
                System.out.println("load native lib from Sys Path: "+"tradeplugin");
                nativeLibHasLoaded.put("tradeplugin", "1");
            }catch (Exception e){
                System.err.println("Fail to load native lib from Sys Path: "+"tradeplugin");
            }
        if(!nativeLibHasLoaded.containsKey("quoteplugin") && !nativeLibHasLoaded.containsKey("libquoteplugin"))
            try{
                System.loadLibrary("quoteplugin");
                System.out.println("load native lib from Sys Path: "+"quoteplugin");
                nativeLibHasLoaded.put("quoteplugin", "1");
            }catch (Exception e){
                System.err.println("Fail to load native lib from Sys Path: "+"quoteplugin");
            }

    }
}
