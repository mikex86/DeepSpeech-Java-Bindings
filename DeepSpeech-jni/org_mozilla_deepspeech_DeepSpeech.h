#ifndef _Included_org_mozilla_deepspeech_NativeImpl
#define _Included_org_mozilla_deepspeech_NativeImpl

#include <jni.h>
#include <string>
#include <string.h>
#include <stdlib.h>
#include <deepspeech.h>

#ifdef __cplusplus
extern "C" {
#endif

enum BuildConfiguration {
    INVALID = -1, CPU = 0, CUDA = 1
};

JNIEXPORT jint JNICALL Java_org_mozilla_deepspeech_DeepSpeech_nCreateModel
        (JNIEnv *, jclass, jstring, jobject);

JNIEXPORT void JNICALL Java_org_mozilla_deepspeech_DeepSpeech_destroyModel
        (JNIEnv *, jclass, jlong);


JNIEXPORT jint JNICALL Java_org_mozilla_deepspeech_DeepSpeech_getModelBeamWidth
        (JNIEnv *, jclass, jlong);

JNIEXPORT jint JNICALL Java_org_mozilla_deepspeech_DeepSpeech_setModelBeamWidth
        (JNIEnv *, jclass, jlong, jlong);

JNIEXPORT jint JNICALL Java_org_mozilla_deepspeech_DeepSpeech_getModelSampleRate
        (JNIEnv *, jclass, jlong);

JNIEXPORT jint JNICALL Java_org_mozilla_deepspeech_DeepSpeech_enableDecoderWithLM
        (JNIEnv *, jclass, jlong, jstring, jfloat, jfloat);

JNIEXPORT jint JNICALL Java_org_mozilla_deepspeech_DeepSpeech_setScorerAlphaBeta
        (JNIEnv *env, jclass, jlong, jfloat, jfloat);

JNIEXPORT jint JNICALL Java_org_mozilla_deepspeech_DeepSpeech_disableExternalScorer
        (JNIEnv *env, jclass, jlong);

JNIEXPORT jstring JNICALL Java_org_mozilla_deepspeech_DeepSpeech_nSpeechToText
        (JNIEnv *, jclass, jlong, jobject, jlong);

JNIEXPORT jstring JNICALL Java_org_mozilla_deepspeech_DeepSpeech_speechToTextUnsafe
        (JNIEnv *, jclass, jlong, jlong, jlong);

JNIEXPORT jlong JNICALL Java_org_mozilla_deepspeech_DeepSpeech_nSpeechToTextWithMetadata
        (JNIEnv *, jclass, jlong, jobject, jlong, jlong);

JNIEXPORT jlong JNICALL Java_org_mozilla_deepspeech_DeepSpeech_speechToTextWithMetadataUnsafe
        (JNIEnv *, jclass, jlong, jlong, jlong, jlong);

JNIEXPORT jint JNICALL Java_org_mozilla_deepspeech_DeepSpeech_nSetupStream
        (JNIEnv *, jclass, jlong, jobject);

JNIEXPORT void JNICALL Java_org_mozilla_deepspeech_DeepSpeech_nFeedAudioContent
        (JNIEnv *, jclass, jlong, jobject, jlong);

JNIEXPORT jstring JNICALL Java_org_mozilla_deepspeech_DeepSpeech_intermediateDecode
        (JNIEnv *, jclass, jlong);

JNIEXPORT jstring JNICALL Java_org_mozilla_deepspeech_DeepSpeech_finishStream
        (JNIEnv *, jclass, jlong);

JNIEXPORT jlong JNICALL Java_org_mozilla_deepspeech_DeepSpeech_finishStreamWithMetadata
        (JNIEnv *, jclass, jlong);

JNIEXPORT void JNICALL Java_org_mozilla_deepspeech_DeepSpeech_discardStream
        (JNIEnv *, jclass, jlong);

JNIEXPORT void JNICALL Java_org_mozilla_deepspeech_DeepSpeech_freeMetadata
        (JNIEnv *, jclass, jlong);

JNIEXPORT jstring JNICALL Java_org_mozilla_deepspeech_DeepSpeech_getVersion
        (JNIEnv *, jclass);

JNIEXPORT jstring JNICALL Java_org_mozilla_deepspeech_DeepSpeech_errorCodeToErrorMessage
        (JNIEnv *, jclass, jlong);

JNIEXPORT jint JNICALL Java_org_mozilla_deepspeech_DeepSpeech_nGetConfiguration
        (JNIEnv *, jclass);

#ifdef __cplusplus
}
#endif
#endif
