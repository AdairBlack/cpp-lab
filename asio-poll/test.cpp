#include <iostream>
#include <memory>
#include <vector>
#include <boost/asio.hpp>

class Comp : public std::enable_shared_from_this<Comp>
{
private:
    std::string comp1;
    std::string comp2;
    std::string comp3;

    boost::asio::io_service &io;
    boost::asio::steady_timer scaningTimer;

public:
    Comp(boost::asio::io_service &io) : io(io), scaningTimer(io)
    {
        comp1 = std::string("awefqwhosalndvjlashdfuovasjdfoj");
        comp2 = std::string("awefqwhosalndvjlashdfuovasjdfoj");
        comp3 = std::string("awefqwhosalndvjlashdfuovasjdfoj");
    }

    ~Comp()
    {
        scaningTimer.cancel();
    }

    void showInfo()
    {
        std::cout << "fuck" << std::endl;
    }

    void run()
    {
        scaningTimer.expires_after(std::chrono::milliseconds(3000));
        std::weak_ptr<Comp> weakRef = weak_from_this();
        scaningTimer.async_wait(
            [this, weakRef](const boost::system::error_code ec)
            {
                std::shared_ptr<Comp> self = weakRef.lock();
                if (ec)
                {
                    // operation_aborted is expected if timer is canceled before
                    // completion.
                    if (ec != boost::asio::error::operation_aborted)
                    {
                        std::cerr << " async_wait failed: " + ec.message() << "\n";
                    }
                    return;
                }
                std::cout << "comp running" << std::endl;
                sleep(15);
                if (self)
                {
                    self->showInfo();
                }
                run();
            });
    }
};

int main(int argc, char *argv[])
{
    boost::asio::io_service io;
    boost::asio::deadline_timer testTimer(io);
    std::vector<Comp> comps;
    comps.emplace_back(std::make_shared<Comp>(io));

    testTimer.expires_from_now(boost::posix_time::seconds(5));

    testTimer.async_wait([&comps](const boost::system::error_code &ec)
                         {
		                    if (ec == boost::asio::error::operation_aborted)
		                    {
			                    return; // we're being canceled
		                    }
		                    std::cout << "Hello World!" << std::endl; 
                            comps.clear(); });

    sleep(1);

    io.run();

    return 0;
}