#include <iostream>
#include "org_mozilla_deepspeech_DeepSpeech.h"

jint
Java_org_mozilla_deepspeech_DeepSpeech_nCreateModel(JNIEnv *env, jclass, jstring modelPath,
                                                    jobject modelStatePtr) {
    jboolean isModelPathCopy;
    ModelState *ptr = nullptr;
    auto modelPathCStr = (char *) env->GetStringUTFChars(modelPath, &isModelPathCopy);

// https://github.com/mozilla/DeepSpeech/commit/8c820817794d445746aefb1b5347b35bf5e0c621#diff-0317a0e76ece10e0dba742af310a2362
    jint state = DS_CreateModel(modelPathCStr, &ptr);

    auto *bufferPtr = (jlong *) (env->GetDirectBufferAddress(modelStatePtr));

    bufferPtr[0] = reinterpret_cast<jlong>(ptr);

    if (isModelPathCopy == JNI_TRUE) {
        env->ReleaseStringUTFChars(modelPath, modelPathCStr);
    }
    return state;
}

void Java_org_mozilla_deepspeech_DeepSpeech_destroyModel(JNIEnv *, jclass, jlong modelPtr) {
    DS_FreeModel(reinterpret_cast<ModelState *>(modelPtr));
}

jint
Java_org_mozilla_deepspeech_DeepSpeech_getModelBeamWidth(JNIEnv *env, jclass, jlong modelStatePtr) {
    return DS_GetModelBeamWidth((ModelState *) modelStatePtr);
}

jint
Java_org_mozilla_deepspeech_DeepSpeech_setModelBeamWidth(JNIEnv *env, jclass, jlong modelStatePtr, jlong beamWidth) {
    return DS_SetModelBeamWidth((ModelState *) modelStatePtr, static_cast<unsigned int>beamWidth);
}

jint
Java_org_mozilla_deepspeech_DeepSpeech_getModelSampleRate(JNIEnv *env, jclass, jlong modelStatePtr) {
    return DS_GetModelSampleRate((ModelState *) modelStatePtr);
}

jint
Java_org_mozilla_deepspeech_DeepSpeech_enableDecoderWithLM(JNIEnv *env, jclass, jlong modelStatePtr,
                                                           jstring scorerPath,
                                                           jfloat alpha, jfloat beta) {
    jboolean isLmPathCopy;
    auto scorerPathCStr = const_cast<char *>(env->GetStringUTFChars(scorerPath, &isLmPathCopy));

    // https://github.com/mozilla/DeepSpeech/pull/2681/files
    // lm: models/lm.binary, trie: models/trie
    // became scorer: models/kenlm.scorer
    DS_EnableExternalScorer((ModelState *) modelStatePtr, scorerPathCStr);
    jint status = DS_SetScorerAlphaBeta((ModelState *) modelStatePtr, alpha, beta);

    if (isLmPathCopy == JNI_TRUE) {
        env->ReleaseStringUTFChars(scorerPath, scorerPathCStr);
    }

    return status;
}

jint
Java_org_mozilla_deepspeech_DeepSpeech_setScorerAlphaBeta(JNIEnv *env, jclass, jlong modelStatePtr,
                                                           jfloat alpha, jfloat beta) {
    return DS_SetScorerAlphaBeta((ModelState *) modelStatePtr, alpha, beta);
}

jint
Java_org_mozilla_deepspeech_DeepSpeech_disableExternalScorer(JNIEnv *env, jclass, jlong modelStatePtr) {
    jint status = DS_DisableExternalScorer((ModelState *) modelStatePtr);
    return status;
}

jstring
Java_org_mozilla_deepspeech_DeepSpeech_nSpeechToText(JNIEnv *env, jclass, jlong modelStatePtr,
                                                     jobject audioBuffer, jlong numSamples) {
    auto *array = (short *) (env->GetDirectBufferAddress(audioBuffer));
    char *cStr = DS_SpeechToText((ModelState *) modelStatePtr, array,
                                 static_cast<unsigned int>(numSamples));
    if (cStr == nullptr) {
        return nullptr;
    }
    jstring str = env->NewStringUTF(cStr);
    DS_FreeString(cStr);
    return str;
}

jstring
Java_org_mozilla_deepspeech_DeepSpeech_speechToTextUnsafe(JNIEnv *env, jclass, jlong modelStatePtr,
                                                     jlong audioBuffer, jlong numSamples) {
    auto *array = (short *) (audioBuffer);
    char *cStr = DS_SpeechToText((ModelState *) modelStatePtr, array,
                                 static_cast<unsigned int>(numSamples));
    if (cStr == nullptr) {
        return nullptr;
    }
    jstring str = env->NewStringUTF(cStr);
    DS_FreeString(cStr);
    return str;
}

jlong
Java_org_mozilla_deepspeech_DeepSpeech_nSpeechToTextWithMetadata(JNIEnv *env, jclass,
                                                                 jlong modelStatePtr,
                                                                 jobject audioBuffer,
                                                                 jlong bufferSize,
                                                                 jlong numResults) {
    auto *array = static_cast<short *>(env->GetDirectBufferAddress(audioBuffer));
    auto metaPtr = reinterpret_cast<jlong>(DS_SpeechToTextWithMetadata((ModelState *) modelStatePtr,
                                                                       array,
                                                                       static_cast<unsigned int>(bufferSize),
                                                                       static_cast<unsigned int>(numResults)));
    return metaPtr;
}
jlong
Java_org_mozilla_deepspeech_DeepSpeech_speechToTextWithMetadataUnsafe(JNIEnv *, jclass,
                                                                 jlong modelStatePtr,
                                                                 jlong audioBuffer,
                                                                 jlong bufferSize,
                                                                 jlong numResults) {
    auto *array = (short *)audioBuffer;
    auto metaPtr = reinterpret_cast<jlong>(DS_SpeechToTextWithMetadata((ModelState *) modelStatePtr,
                                                                       array,
                                                                       static_cast<unsigned int>(bufferSize),
                                                                       static_cast<unsigned int>(numResults)));
    return metaPtr;
}

jint Java_org_mozilla_deepspeech_DeepSpeech_nSetupStream(JNIEnv *env, jclass, jlong modelStatePtr,
                                                         jobject streamPtr) {
    StreamingState *pStreamingState;

    jint status = DS_CreateStream((ModelState *) modelStatePtr, &pStreamingState);

    auto p = (StreamingState **) env->GetDirectBufferAddress(streamPtr);
    *p = pStreamingState;
    return status;
}

void Java_org_mozilla_deepspeech_DeepSpeech_nFeedAudioContent(JNIEnv *env, jclass, jlong streamPtr,
                                                              jobject audioBuffer,
                                                              jlong bufferSize) {
    auto *test = static_cast<short *>(env->GetDirectBufferAddress(audioBuffer));
    DS_FeedAudioContent((StreamingState *) streamPtr, test, static_cast<unsigned int>(bufferSize));
}

jstring
Java_org_mozilla_deepspeech_DeepSpeech_intermediateDecode(JNIEnv *env, jclass, jlong streamPtr) {
    char *cString = DS_IntermediateDecode((StreamingState *) streamPtr);
    jstring str = env->NewStringUTF(cString);
    DS_FreeString(cString);
    return str;
}

jstring Java_org_mozilla_deepspeech_DeepSpeech_finishStream(JNIEnv *env, jclass, jlong streamPtr) {
    char *cString = DS_FinishStream((StreamingState *) streamPtr);
    size_t cStrLen = strlen(cString);
    jstring str = env->NewString(reinterpret_cast<const jchar *>(cString),
                                 static_cast<jsize>(cStrLen));
    DS_FreeString(cString);
    return str;
}

jlong
Java_org_mozilla_deepspeech_DeepSpeech_finishStreamWithMetadata(JNIEnv *, jclass, jlong streamPtr, jlong numResults) {
    return reinterpret_cast<jlong>(DS_FinishStreamWithMetadata((StreamingState *) streamPtr, numResults));
}

void Java_org_mozilla_deepspeech_DeepSpeech_discardStream(JNIEnv *, jclass, jlong streamPtr) {
    DS_FreeStream((StreamingState *) streamPtr);
}

void Java_org_mozilla_deepspeech_DeepSpeech_freeMetadata(JNIEnv *, jclass, jlong metaPtr) {
    DS_FreeMetadata((Metadata *) metaPtr);
}

jstring Java_org_mozilla_deepspeech_DeepSpeech_getVersion(JNIEnv *, jclass) {
    char *cString = DS_Version();
    size_t cStrLen = strlen(cString);
    jstring str = env->NewString(reinterpret_cast<const jchar *>(cString),
                                     static_cast<jsize>(cStrLen));
    DS_FreeString(cString);
    return str;
}

jstring Java_org_mozilla_deepspeech_DeepSpeech_errorCodeToErrorMessage(JNIEnv *, jclass, jlong errorCode) {
    char *cString = DS_ErrorCodeToErrorMessage(errorCode);
    size_t cStrLen = strlen(cString);
    jstring str = env->NewString(reinterpret_cast<const jchar *>(cString),
                                     static_cast<jsize>(cStrLen));
    DS_FreeString(cString);
    return str;
}

jint Java_org_mozilla_deepspeech_DeepSpeech_nGetConfiguration(JNIEnv *, jclass) {
#ifdef CPU_BUILD_CONFIG
    return BuildConfiguration::CPU;
#else
#ifdef CUDA_BUILD_CONFIG
    return BuildConfiguration::CUDA;
#else
    return BuildConfiguration::INVALID; // This should never be returned
#endif
#endif
}
