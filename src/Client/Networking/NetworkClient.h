#pragma once

#include "../../Common/NetCommon.h"
#include "../../Core/Connection.h"

using asio::ip::tcp;

class NetworkClient {
public:
    Signal<const std::vector<uint8_t>&> OnServerMessage;
    Signal<> OnConnected;
    Signal<> OnDisconnected;
    Signal<std::string> OnConnectFailed;

    explicit NetworkClient(asio::io_context& io)
        : socket(io), resolver(io), strand(asio::make_strand(io)), timeoutTimer(io) {}

    void connect(const std::string& host, unsigned short port, std::chrono::seconds timeout = std::chrono::seconds(5)) {
        asio::dispatch(strand, [this, host, port, timeout]() {
            if (connected) {
                std::cerr << "[NetworkClient] Already connected.\n";
                return;
            }

            std::cout << "[NetworkClient] Resolving " << host << ":" << port << "...\n";

            resolver.async_resolve(host, std::to_string(port),
                asio::bind_executor(strand,
                    [this, timeout](std::error_code ec, tcp::resolver::results_type results) {
                        if (ec) {
                            std::string msg = "[NetworkClient] Resolve failed: " + ec.message();
                            std::cerr << msg << "\n";
                            OnConnectFailed.Fire(msg);
                            return;
                        }

                        // Start connection attempt
                        asio::async_connect(socket, results,
                            asio::bind_executor(strand,
                                [this](std::error_code ec, tcp::endpoint ep) {
                                    timeoutTimer.cancel(); // cancel timeout when connect finishes
                                    if (!ec) {
                                        connected.store(true);
                                        std::cout << "[NetworkClient] Connected to " << ep << "\n";
                                        OnConnected.Fire();
                                        readLoop();
                                    } else {
                                        std::string msg = "[NetworkClient] Connect failed: " + ec.message();
                                        std::cerr << msg << "\n";
                                        OnConnectFailed.Fire(msg);
                                    }
                                }));

                        // Start connection timeout
                        timeoutTimer.expires_after(timeout);
                        timeoutTimer.async_wait(asio::bind_executor(strand,
                            [this](const std::error_code& ec) {
                                if (!ec) {
                                    if (!connected.load()) {
                                        std::cerr << "[NetworkClient] Connection timed out.\n";
                                        std::error_code ignored;
                                        socket.cancel(ignored);
                                        OnConnectFailed.Fire("Connection timed out.");
                                    }
                                }
                            }));
                    }));
        });
    }

    void disconnect() {
        asio::post(strand, [this]() {
            timeoutTimer.cancel();
            if (!connected.exchange(false))
                return;

            std::error_code ec;
            socket.shutdown(tcp::socket::shutdown_both, ec);
            socket.close(ec);
            if (ec)
                std::cerr << "[NetworkClient] Disconnect error: " << ec.message() << "\n";
            OnDisconnected.Fire();
            std::cout << "[NetworkClient] Disconnected cleanly.\n";
        });
    }

    void send(const std::vector<uint8_t>& data) {
        asio::post(strand, [this, data]() {
            if (!connected.load()) {
                std::cerr << "[NetworkClient] Cannot send: not connected.\n";
                return;
            }
            bool write_in_progress = !outgoing.empty();
            outgoing.emplace_back(data.begin(), data.end());
            if (!write_in_progress)
                doWrite();
        });
    }

    bool isConnected() const { return connected.load(); }

private:
    void doWrite() {
        asio::async_write(socket,
            asio::buffer(outgoing.front()),
            asio::bind_executor(strand,
                [this](std::error_code ec, std::size_t /*len*/) {
                    if (!ec) {
                        outgoing.erase(outgoing.begin());
                        if (!outgoing.empty())
                            doWrite();
                    } else {
                        std::cerr << "[NetworkClient] Write failed: " << ec.message() << "\n";
                        handleDisconnect(ec);
                    }
                }));
    }

    void readLoop() {
        socket.async_read_some(asio::buffer(buffer),
            asio::bind_executor(strand,
                [this](std::error_code ec, std::size_t len) {
                    if (!ec) {
                        std::vector<uint8_t> msg(buffer.data(), buffer.data() + len);
                        OnServerMessage.Fire(msg);
                        readLoop();
                    } else {
                        std::cerr << "[NetworkClient] Read error: " << ec.message() << "\n";
                        handleDisconnect(ec);
                    }
                }));
    }

    void handleDisconnect(const std::error_code& ec) {
        if (connected.exchange(false)) {
            std::cerr << "[NetworkClient] Disconnected (" << ec.message() << ")\n";
            std::error_code close_ec;
            socket.close(close_ec);
            if (close_ec)
                std::cerr << "[NetworkClient] Socket close error: " << close_ec.message() << "\n";
            OnDisconnected.Fire();
        }
    }

    std::atomic<bool> connected = false;
    tcp::socket socket;
    tcp::resolver resolver;
    asio::strand<asio::io_context::executor_type> strand;
    asio::steady_timer timeoutTimer;
    std::array<char, 2048> buffer;
    std::vector<std::string> outgoing;
};