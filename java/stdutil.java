public class stdutil{

    static {
        System.loadLibrary("java.dll");
    }

    public native String downloadfile(String url, boolean filewrite, String outputPath);
    public native void frepy(Object thing);
}