# DeepSpeech android library

The DeepSpeech android library `deepspeechandroid.aar` are the android bindings to access the native DeepSpeech
library. While the `deepspeechandroid.aar` library isn't really different to the normal Java bindings,
it does come in handy for Android developers, as the natives are placed inside the `.aar` archive.
The `deepspeechandroid` project is really just compiling the `libdeepspeech` java bindings and building the JNI for supported ABIs.

When the library is compiled using gradle, they are automatically packaged correctly inside your APK and
you do not have to worry about them not getting found. To find out how to load the DeepSpeech native library, have a
look at the `Readme.md` of the `libdeepspeech` project.
 
## Building the `deepspeechandroid.aar` library

### Important notice

First you need to make sure that the JNI CMake project is setup correctly.
It is recommended to do a test build of the `libdeepspeech-jni` library
first before building the AAR. Instructions on how to build the JNI library
can be found in the `README.MD` file of the project.

Run the following command from the `AndroidTest` project directory

```
gradle build
```

The `deepspeechandroid.aar` _(both debug and release)_ libraries will be placed in `./deepspeechandroid/build/outputs/aar/`.

