#ifndef WEBRTCCONNECTION_H_
#define WEBRTCCONNECTION_H_

#include <string>
#include <queue>
#include <boost/thread/mutex.hpp>
#include <boost/thread.hpp>

#include "SrtpChannel.h"
#include "SdpInfo.h"
#include "MediaDefinitions.h"
#include "Transport.h"

namespace erizo {

class Transport;
class TransportListener;

/**
 * States of ICE
 */
enum WebRTCState {
	INITIAL, STARTED, READY, FINISHED, FAILED
};


class WebRtcConnectionStateListener {
public:
	virtual ~WebRtcConnectionStateListener() {
	}
	;
	virtual void connectionStateChanged(WebRTCState newState)=0;

};

/**
 * A WebRTC Connection. This class represents a WebRTC Connection that can be established with other peers via a SDP negotiation
 * it comprises all the necessary Transport components.
 */
class WebRtcConnection: public MediaSink, public MediaSource, public FeedbackSink, public FeedbackSource, public TransportListener {
public:
	/**
	 * Constructor.
	 * Constructs an empty WebRTCConnection without any configuration.
	 */
	WebRtcConnection();
	/**
	 * Destructor.
	 */
	virtual ~WebRtcConnection();
	/**
	 * Inits the WebConnection by starting ICE Candidate Gathering.
	 * @return True if the candidates are gathered.
	 */
	bool init();
	/**
	 * Closes the webRTC connection.
	 * The object cannot be used after this call.
	 */
	void close();
  void closeSink();
  void closeSource();
	/**
	 * Sets the SDP of the remote peer.
	 * @param sdp The SDP.
	 * @return true if the SDP was received correctly.
	 */
	bool setRemoteSdp(const std::string &sdp);
	/**
	 * Obtains the local SDP.
	 * @return The SDP as a string.
	 */
	std::string getLocalSdp();

	int deliverAudioData(char* buf, int len);
	int deliverVideoData(char* buf, int len);

    int deliverFeedback(char* buf, int len);

	/**
	 * Sends a FIR Packet (RFC 5104) asking for a keyframe
	 * @return the size of the data sent
	 */
	int sendFirPacket();

	void setWebRTCConnectionStateListener(
			WebRtcConnectionStateListener* listener);
	/**
	 * Gets the current state of the Ice Connection
	 * @return
	 */
	WebRTCState getCurrentState();

	void onTransportData(char* buf, int len, bool rtcp, unsigned int recvSSRC, Transport *transport);

	void updateState(TransportState state, Transport * transport);

	void queueData(const char* data, int len);

private:
	SdpInfo remoteSdp_;
	SdpInfo localSdp_;

	WebRTCState globalState_;

	int video_, audio_, bundle_, sequenceNumberFIR_;
	boost::mutex writeMutex_, receiveAudioMutex_, receiveVideoMutex_, updateStateMutex_;
	boost::thread send_Thread_;
	std::queue<dataPacket> sendQueue_;
	WebRtcConnectionStateListener* connStateListener_;
	Transport *videoTransport_, *audioTransport_;

	bool sending;
	void sendLoop();

};

} /* namespace erizo */
#endif /* WEBRTCCONNECTION_H_ */
