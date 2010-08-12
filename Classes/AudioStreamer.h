// 
// 元となったコードは、
// Matt Gallagher の書いたコードを手直ししたもの
// http://www.smartsdktips.com/blog/2010/03/14/iphone-audio-streamer-working-code-with-call-interruption/
//

#ifdef TARGET_OS_IPHONE			
#import <UIKit/UIKit.h>
// #define USE_HTTP_STREAMING							// HTTPストリーミングする(プロジェクトにCFNetwork.fremaworkを加える事)
#else
#import <Cocoa/Cocoa.h>
#endif TARGET_OS_IPHONE			

#ifdef DEBUG
	#define PRINTF(...)	printf(__VA_ARGS__)
	#define NSLOG(...)	NSLog(__VA_ARGS__)
#else
	#define PRINTF(...)
	#define NSLOG(...)
#endif

#include <pthread.h>
#include <AudioToolbox/AudioToolbox.h>

#define LOG_QUEUED_BUFFERS 0

#define kNumAQBufs 16			// Number of audio queue buffers we allocate.
// Needs to be big enough to keep audio pipeline
// busy (non-zero number of queued buffers) but
// not so big that audio takes too long to begin
// (kNumAQBufs * kAQBufSize of data must be
// loaded before playback will start).
// Set LOG_QUEUED_BUFFERS to 1 to log how many
// buffers are queued at any time -- if it drops
// to zero too often, this value may need to
// increase. Min 3, typical 8-24.

#define kAQBufSize 2048			// Number of bytes in each audio queue buffer
// Needs to be big enough to hold a packet of
// audio from the audio file. If number is too
// large, queuing of audio before playback starts
// will take too long.
// Highly compressed files can use smaller
// numbers (512 or less). 2048 should hold all
// but the largest packets. A buffer size error
// will occur if this number is too small.

#define kAQMaxPacketDescs 512	// Number of packet descriptions in our array

typedef enum
	{
		AS_INITIALIZED = 0,
		AS_STARTING_FILE_THREAD,
		AS_WAITING_FOR_DATA,
		AS_WAITING_FOR_QUEUE_TO_START,
		AS_PLAYING,
		AS_BUFFERING,
		AS_STOPPING,
		AS_STOPPED,
		AS_PAUSED
	} AudioStreamerState;

typedef enum
	{
		AS_NO_STOP = 0,
		AS_STOPPING_EOF,
		AS_STOPPING_USER_ACTION,
		AS_STOPPING_ERROR,
		AS_STOPPING_TEMPORARILY
	} AudioStreamerStopReason;

typedef enum
	{
		AS_NO_ERROR = 0,
		AS_NETWORK_CONNECTION_FAILED,
		AS_FILE_STREAM_GET_PROPERTY_FAILED,
		AS_FILE_STREAM_SEEK_FAILED,
		AS_FILE_STREAM_PARSE_BYTES_FAILED,
		AS_FILE_STREAM_OPEN_FAILED,
		AS_FILE_STREAM_CLOSE_FAILED,
		AS_AUDIO_DATA_NOT_FOUND,
		AS_AUDIO_QUEUE_CREATION_FAILED,
		AS_AUDIO_QUEUE_BUFFER_ALLOCATION_FAILED,
		AS_AUDIO_QUEUE_ENQUEUE_FAILED,
		AS_AUDIO_QUEUE_ADD_LISTENER_FAILED,
		AS_AUDIO_QUEUE_REMOVE_LISTENER_FAILED,
		AS_AUDIO_QUEUE_START_FAILED,
		AS_AUDIO_QUEUE_PAUSE_FAILED,
		AS_AUDIO_QUEUE_BUFFER_MISMATCH,
		AS_AUDIO_QUEUE_DISPOSE_FAILED,
		AS_AUDIO_QUEUE_STOP_FAILED,
		AS_AUDIO_QUEUE_FLUSH_FAILED,
		AS_AUDIO_STREAMER_FAILED,
		AS_GET_AUDIO_TIME_FAILED,
		AS_AUDIO_BUFFER_TOO_SMALL,
		AS_NETWORK_CONFIG_FAILED,
		AS_STREAM_CREATE_FAILED,
		AS_STREAM_OPEN_FAILED,
	} AudioStreamerErrorCode;

@interface AudioStreamer : NSObject
{
	NSURL *url;
	
	//
	// Special threading consideration:
	//	The audioQueue property should only ever be accessed inside a
	//	synchronized(self) block and only *after* checking that ![self isFinishing]
	//
	AudioQueueRef	  audioQueue;
	AudioFileStreamID audioFileStream;				// the audio file stream parser
	
	AudioQueueBufferRef audioQueueBuffer[kNumAQBufs];			 // audio queue buffers
	AudioStreamPacketDescription packetDescs[kAQMaxPacketDescs];	// packet descriptions for enqueuing audio
	unsigned int fillBufferIndex;	// the index of the audioQueueBuffer that is being filled
	size_t bytesFilled;				// how many bytes have been filled
	size_t packetsFilled;			// how many packets have been filled
	bool inuse[kNumAQBufs];			// flags to indicate that a buffer is still in use
	NSInteger buffersUsed;
	
	AudioStreamerState state;
	AudioStreamerState pause_state;
	AudioStreamerStopReason stopReason;
	AudioStreamerErrorCode errorCode;
	bool discontinuous;								// flag to indicate middle of the stream

	OSStatus err;
	pthread_mutex_t queueBuffersMutex;				// a mutex to protect the inuse flags
	pthread_cond_t queueBufferReadyCondition;		// a condition varable for handling the inuse flags
	
	CFReadStreamRef stream;
	NSNotificationCenter *notificationCenter;
	
	UInt32 bitRate;
	
	double sampleRate;
	double lastProgress;

	BOOL loopStreaming;
}
@property (readonly) AudioStreamerErrorCode errorCode;
@property (readonly) AudioStreamerState state;
@property (readonly) double progress;

- (id)initWithURL:(NSURL *)aURL;
- (void)start;
- (void)stop:(BOOL)aWait;
- (void)pause;
- (BOOL)isPlaying;
- (BOOL)isPaused;
- (BOOL)isWaiting;
- (BOOL)isIdle;

- (id)initWithFile:(NSString *)aPath;
- (void)looping:(BOOL)aLoop;

@end
