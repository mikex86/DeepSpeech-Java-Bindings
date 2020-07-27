package org.mozilla.deepspeech.recognition.stream;

import org.jetbrains.annotations.NotNull;
import org.mozilla.deepspeech.DeepSpeech;
import org.mozilla.deepspeech.doc.Calls;
import org.mozilla.deepspeech.doc.NativeType;
import org.mozilla.deepspeech.exception.buffer.BufferReadonlyException;
import org.mozilla.deepspeech.exception.buffer.IncorrectBufferByteOrderException;
import org.mozilla.deepspeech.exception.buffer.IncorrectBufferTypeException;
import org.mozilla.deepspeech.exception.buffer.UnexpectedBufferCapacityException;
import org.mozilla.deepspeech.nativewrapper.DynamicStruct;
import org.mozilla.deepspeech.recognition.DeepSpeechModel;
import org.mozilla.deepspeech.utils.BufferUtils;
import org.mozilla.deepspeech.utils.NativeAccess;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

import static org.mozilla.deepspeech.DeepSpeech.*;

public class StreamingState extends DynamicStruct.LifecycleDisposed {

    /**
     * Create a new streaming inference state
     */
    public static StreamingState setupStream(@NotNull DeepSpeechModel model) {
        ByteBuffer streamPointer = ByteBuffer.allocateDirect(NativeAccess.NATIVE_POINTER_SIZE).order(ByteOrder.nativeOrder());

        long status = DeepSpeech.setupStream(model.getPointer(), streamPointer);
        if (status != ErrorCodes.OK) {
            throw new IllegalStateException(errorCodeToErrorMessage(status));
        }

        return new StreamingState(BufferUtils.getBufferAddress(streamPointer));
    }

    /**
     * Feed audio samples to an ongoing streaming inference.
     *
     * @param audioBuffer   An array of 16-bit, mono raw audio samples at the appropriate sample rate.
     * @param numSamples    The number of samples in the audio content.
     * @throws UnexpectedBufferCapacityException if #numSamples does not match the allocated buffer capacity. Condition: {@code numSamples * Short.BYTES < audioBuffer.capacity()}
     * @throws IncorrectBufferByteOrderException if the audioBuffer has a byte order different to {@link ByteOrder#nativeOrder()}.
     * @throws IncorrectBufferTypeException      if the audioBuffer is not directly allocated.
     * @throws BufferReadonlyException           if the buffer is read only
     */
    public void feedAudioContent(@NotNull ByteBuffer audioBuffer,
                                 long numSamples) throws UnexpectedBufferCapacityException, IncorrectBufferByteOrderException, IncorrectBufferTypeException, BufferReadonlyException {
        DeepSpeech.feedAudioContent(this.pointer, audioBuffer, numSamples);
    }

    /**
     * Compute the intermediate decoding of an ongoing streaming inference.
     * This is an expensive process as the decoder implementation isn't currently capable of streaming,
     * so it always starts from the beginning of the audio.
     *
     * @return The STT intermediate result.
     */
    public String intermediateDecode() {
        return DeepSpeech.intermediateDecode(this.pointer);
    }

    /**
     * This method will free the state pointer (#pointer)
     * Signal the end of an audio signal to an ongoing streaming
     * inference, returns the STT result over the whole audio signal.
     *
     * @return The STT result.
     */
    @Calls("DS_FinishStream")
    @NativeType("jstring")
    public String finishStream() {
        return DeepSpeech.finishStream(this.pointer);
    }

// TODO: org_mozilla_deepspeech_DeepSpeech.cpp needs to return a DynamicStruct that can be parsed to a SpeechRecognitionResult.SpokenCharacterData
//    /**
//     * This method will free the state pointer #streamPointer.
//     * Signal the end of an audio signal to an ongoing streaming inference, returns per-letter metadata.
//     *
//     * @param numResults The number of candidate transcripts to return.
//     * @return Outputs a struct of individual letters along with their timing information.
//     * The user is responsible for freeing Metadata by calling {@link #freeMetadata(long)}.
//     * Returns {@link #NULL} on error.
//     */
//    public long finishStreamWithMetadata(long numResults) {
//        return DeepSpeech.finishStreamWithMetadata(this.streamPointer, numResults);
//    }
//    public void freeMetadata(long metaDataPointer) {
//        DeepSpeech.freeMetadata(metaDataPointer);
//    }

    @Override
    protected void deallocateStruct(long pointer) {
        discardStream(pointer);
    }

    private StreamingState(long streamPointer) {
        super(streamPointer, NativeAccess.NATIVE_POINTER_SIZE);
    }
}
