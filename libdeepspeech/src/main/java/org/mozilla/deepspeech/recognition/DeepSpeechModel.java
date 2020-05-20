package org.mozilla.deepspeech.recognition;

import org.jetbrains.annotations.NotNull;
import org.mozilla.deepspeech.DeepSpeech;
import org.mozilla.deepspeech.doc.NativeType;
import org.mozilla.deepspeech.doc.WrappsStruct;
import org.mozilla.deepspeech.exception.buffer.BufferReadonlyException;
import org.mozilla.deepspeech.exception.buffer.IncorrectBufferByteOrderException;
import org.mozilla.deepspeech.exception.buffer.IncorrectBufferTypeException;
import org.mozilla.deepspeech.exception.buffer.UnexpectedBufferCapacityException;
import org.mozilla.deepspeech.nativewrapper.DynamicStruct;
import org.mozilla.deepspeech.recognition.stream.StreamingState;

import java.io.File;
import java.io.FileNotFoundException;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;

import static org.mozilla.deepspeech.DeepSpeech.*;
import static org.mozilla.deepspeech.utils.BufferUtils.getBufferAddress;
import static org.mozilla.deepspeech.utils.NativeAccess.NATIVE_POINTER_SIZE;
import static org.mozilla.deepspeech.utils.NativeAccess.getNativePointer;
import static org.mozilla.deepspeech.utils.UtilFunctions.checkExists;

/**
 * Represents a trained deep speech recognition recognition
 */
@WrappsStruct("ModelState")
public class DeepSpeechModel extends DynamicStruct.LifecycleDisposed {

    /**
     * @param modelFile          The file pointing to the frozen recognition graph.
     * @param beamWidth          The beam width used by the decoder. A larger beam
     *                           width generates better results at the cost of decoding
     *                           time.
     * @throws FileNotFoundException if modelFile is not found
     */
    public DeepSpeechModel(@NotNull File modelFile, long beamWidth) throws FileNotFoundException {
        super(newModel(modelFile, beamWidth), UNDEFINED_STRUCT_SIZE);
    }

    /**
     * Get beam width value used by the model. If {@link #setBeamWidth(long)} was not called before,
     * will return the default value loaded from the model file.
     *
     * @return Beam width value used by the model.
     */
    public int getBeamWidth() {
        return getModelBeamWidth(this.pointer);
    }

    /**
     * Set beam width value used by the model.
     *
     * @param beamWidth The beam width used by the model. A larger beam width value
     *                   generates better results at the cost of decoding time.
     *
     * @return Zero on success, non-zero on failure.
     */
    public int setBeamWidth(long beamWidth) {
        return setModelBeamWidth(this.pointer, beamWidth);
    }

    /**
     * Return the sample rate expected by a model.
     *
     * @return Sample rate expected by the model for its input.
     */
    public int getSampleRate() {
        return getModelSampleRate(this.pointer);
    }

    /**
     * Enables decoding using beam scoring with a KenLM language model.
     *
     * @param scorerPath   The path to the scorer package generated with `data/lm/generate_package.py`.
     * @param lmAlpha      The alpha hyper-parameter of the CTC decoder. Language Model weight.
     * @param lmBeta       The beta hyper-parameter of the CTC decoder. Word insertion weight.
     * @throws FileNotFoundException if the file is not found.
     */
    public void enableLMLanguageModel(@NotNull File scorerPath, float lmAlpha, float lmBeta) throws FileNotFoundException {
        DeepSpeech.enableDecoderWithLM(this.pointer, checkExists(scorerPath).getPath(), lmAlpha, lmBeta);
    }

    /**
     * @param lmAlpha      The alpha hyper-parameter of the CTC decoder. Language Model weight.
     * @param lmBeta       The beta hyper-parameter of the CTC decoder. Word insertion weight.
     */
    public void setScorerAlphaBeta(float lmAlpha, float lmBeta) throws FileNotFoundException {
        DeepSpeech.setScorerAlphaBeta(this.pointer, lmAlpha, lmBeta);
    }

    /**
     * Disable decoding using an external scorer.
     */
    public void disableExternalScorer(@NotNull File scorerPath, float lmAlpha, float lmBeta) throws FileNotFoundException {
        DeepSpeech.disableExternalScorer(this.pointer);
    }

    /**
     * Performs a text to speech call on the recognition
     *
     * @param audioBuffer the 16-bit, mono raw audio buffer storing the audio data at the appropriate sample rate
     *                    (matching what the model was trained on).
     * @param numSamples  the number of samples / frames in the buffer
     * @return the transcription string
     * @throws UnexpectedBufferCapacityException if #numSamples does not match the allocated buffer capacity. Condition: {@code numSamples * Short.BYTES > audioBuffer.capacity()}
     * @throws IncorrectBufferByteOrderException if the audioBuffer has a byte order different to {@link ByteOrder#nativeOrder()}.
     * @throws IncorrectBufferTypeException      if the audioBuffer is not directly allocated.
     * @throws BufferReadonlyException           if the buffer is read only
     */
    @NotNull
    public String doSpeechToText(@NativeType("const short *") @NotNull ByteBuffer audioBuffer, long numSamples, long sampleRate) throws UnexpectedBufferCapacityException, IncorrectBufferByteOrderException, IncorrectBufferTypeException, BufferReadonlyException {
        String ret = speechToText(this.pointer, audioBuffer, numSamples);
        if (ret == null) throw new NullPointerException();
        return ret;
    }

    /**
     * Performs a text to speech call on the recognition and returns a more detailed STT result with additional meta data - not just a sting
     *
     * @param audioBuffer the audio buffer storing the audio data in samples / frames to perform the recognition on
     * @param numSamples  the number of samples / frames in the buffer
     * @param numResults  The maximum number of CandidateTranscript structs to return. Returned value might be smaller than this.
     * @return the meta data of transcription
     * @see SpeechRecognitionResult
     */
    @NotNull
    public SpeechRecognitionResult doSpeechRecognitionWithMeta(@NativeType("const short *") @NotNull ByteBuffer audioBuffer, long numSamples, long numResults) {
        long metaPointer = speechToTextWithMetadata(this.pointer, audioBuffer, numSamples, numResults);
        if (metaPointer == NULL) throw new NullPointerException();
        return new SpeechRecognitionResult(metaPointer); // Meta pointer is freed as Recognition Result instantly disposes it after copying the values.
    }

    /**
     * Allocates a new native recognition structure and returns the pointer pointing to the dynamically allocated memory
     *
     * @see DeepSpeechModel#DeepSpeechModel(File, long)
     */
    private static long newModel(@NotNull File modelFile, long beamWidth) throws FileNotFoundException {
        ByteBuffer ptr = ByteBuffer.allocateDirect(NATIVE_POINTER_SIZE).order(ByteOrder.LITTLE_ENDIAN);
        if (createModel(checkExists(modelFile).getPath(), beamWidth, ptr) != 0)
            throw new RuntimeException("Failed to create recognition!");
        return getNativePointer(getBufferAddress(ptr));
    }

    /**
     * De-allocates the struct memory when this object is garbage collected
     */
    @Override
    protected void deallocateStruct(long pointer) {
        destroyModel(pointer);
    }

    /**
     * @return the pointer to the native struct below
     */
    public long getPointer() {
        return this.pointer;
    }

    /**
     * Unsafe function! Consider using {@link #doSpeechToText(ByteBuffer, long, long)} instead.
     * Performs a text to speech call on the recognition.
     *
     * @param audioBufferPointer the audio buffer storing the audio data in samples / frames to perform the recognition on
     * @param numSamples         the number of samples / frames in the buffer
     * @return the transcription string
     */
    @NotNull
    public String doSpeechToTextUnsafe(@NativeType("const short *") long audioBufferPointer, long numSamples) {
        String ret = speechToTextUnsafe(this.pointer, audioBufferPointer, numSamples);
        if (ret == null) throw new NullPointerException();
        return ret;
    }

    /**
     * Unsafe function! Consider using {@link #doSpeechRecognitionWithMeta(ByteBuffer, long, long)} instead.
     *
     * @param audioBufferPointer the audio buffer storing the audio data in samples / frames to perform the recognition on
     * @param numSamples         the number of samples / frames in the buffer
     * @param numResults         The maximum number of CandidateTranscript structs to return. Returned value might be smaller than this.
     * @return the meta data of transcription
     * @see SpeechRecognitionResult
     */
    @NotNull
    public SpeechRecognitionResult doSpeechRecognitionWithMetaUnsafe(@NativeType("const short *") long audioBufferPointer, long numSamples, long numResults) {
        long metaPtr = speechToTextWithMetadataUnsafe(this.pointer, audioBufferPointer, numSamples, numResults);
        if (metaPtr == NULL) throw new NullPointerException();
        return new SpeechRecognitionResult(metaPtr); // MetaPtr is freed after this action
    }
}
