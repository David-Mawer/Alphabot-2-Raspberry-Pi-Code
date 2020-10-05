/*
    So - this is my attempt to get blue-tooh in an easy-to-use class.
    After some digging - I found some documentation on the bluez / libbluetooth-dev classes.
    at this URL
        https://man7.org/linux/man-pages/man7/socket.7.html

    so - if the code makes calls that you don't know - then check out the web page -
    they're all documented on that site.
*/


#include <iostream>
#include "bt_handler.h"
#include <unistd.h>
#include <atomic>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>
#include <fcntl.h>    /* For O_RDWR */
#include <poll.h>
#include <appState.h>

bt_handler::bt_handler(TBtResultCallback cb)
{
    //ctor
    m_result_callback = cb;
}

std::string bt_handler::convertToString(char* a)
{
    std::string s(a);
    return s;
}

void bt_handler::run() {
    appState* mypState = appState::myInstance();

    int bt_soc; // the RFCOMM socket for bluetooth.

    // allocate RFCOMM socket (int socket_family, int socket_type, int protocol)
    bt_soc = socket(AF_BLUETOOTH, SOCK_STREAM | SOCK_NONBLOCK, BTPROTO_RFCOMM);

    if (bt_soc < 0) {
        // TODO: Consider setting up a callback that tells the main thread
        //          that blue-tooth monitoring is not possible.
        exit(bt_soc);
    }

    // bind socket to port 1 of the first available local bluetooth adapter
    struct sockaddr_rc local_bt_socket = { 0 };
    local_bt_socket.rc_family = AF_BLUETOOTH;
    local_bt_socket.rc_channel = (uint8_t) 1;
    bind(bt_soc, (struct sockaddr *)&local_bt_socket, sizeof(local_bt_socket));

    // put socket into listening mode
    listen(bt_soc, 1);

    // set up variables used in the loop.
    struct sockaddr_rc remote_bt_socket = { 0 }; // socket for the blue-tooth client.
    char btBuf[1024] = { 0 };
    int client, bytes_read, poll_result;
    struct pollfd pclient;
    socklen_t opt = sizeof(remote_bt_socket);
    std::string sBtValue;

    while (mypState->isBotActive()) {
        // extractsthe first connection request on the queue of pending connections for the listening socket
        // TODO: Find a way to do this that doesn't lock the thread.
        client = accept(bt_soc, (struct sockaddr *)&remote_bt_socket, &opt);
        pclient.fd = client;
        pclient.events = POLLIN;
        if (client < 0) {

            // no connection waiting - so just try again in a second.
            usleep(1000000);

        } else {
            // managed to get a connection from the listening socket.

            // set the client to have non-blocking reads
            int flags = fcntl(client, F_GETFL, 0);
            fcntl(client, F_SETFL, flags | O_NONBLOCK);

            // find the client's (remote socket's) MAC.
            ba2str( &remote_bt_socket.rc_bdaddr, btBuf );
            fprintf(stderr, "accepted connection from %s\n", btBuf);
            // TODO: Consider setting up a callback that tells the main thread
            //          that we have a new connection.

            // loop - reading any inputs from the client until something interrupts.
            bytes_read = 1; // just to kick off the loop.
            while (mypState->isBotActive() && bytes_read > 0) {
                // Monitor the blue-tooth connection

                // checked what's happening on the connection (non-blocking call)
                // poll(...) is nicely documented at
                //      https://man7.org/linux/man-pages/man2/poll.2.html
                poll_result = poll(&pclient, 1, 1000);

                if (poll_result != -1) {
                    // check went OK - now to interpret the results.
                    bool gotChar = (pclient.revents & POLLIN);
                    bool noConnect = (pclient.revents & POLLHUP);
                    bool gotError = (pclient.revents & POLLERR);

                    if (gotChar) {

                        // there's something for me to read
                        memset(btBuf, 0, sizeof(btBuf));
                        bytes_read = read(client, btBuf, sizeof(btBuf));
                        if( bytes_read > 0 ) {
                            // pass the string read back to the main thread.
                            sBtValue = convertToString(btBuf);
                            if (m_result_callback != nullptr) {
                                m_result_callback(sBtValue);
                            }
                            usleep(10000);
                        }

                    } else if (noConnect) {
                        // disconnected - exit the loop so we can wait for another connection.
                        bytes_read = 0;
                    } else if (gotError) {
                        // got an error - assume disconnected (until debugging shows otherwise).
                        std::cout << "Error poll_result.revents: " << pclient.revents <<"...\n ASSUMING DISCONNECTED. \n";
                        bytes_read = 0;
                    } else {
                        // got nothing - just wait and try again.
                        usleep(10000);
                    }
                } else {
                    // poll failed - assume connection gone.
                    bytes_read = 0;
                }
            }

            // TODO: Consider setting up a callback that tells the main thread
            //          that the connection is a gonner.


        }

        // Close the bluetooth connection
        close(client);
    }

    // close the blue-tooth socket.
    close(bt_soc);

    // TODO: Consider setting up a callback that tells the main thread that bluetooth is not being monitored anymore.
}
