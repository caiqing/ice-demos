// **********************************************************************
//
// Copyright (c) 2003-2017 ZeroC, Inc. All rights reserved.
//
// **********************************************************************

#include <Ice/Ice.h>
#include <Hello.h>

using namespace std;

class Callback : public IceUtil::Shared
{
public:

    void response()
    {
    }

    void exception(const Ice::Exception& ex)
    {
        cerr << "sayHello AMI call failed:\n" << ex << endl;
    }
};
typedef IceUtil::Handle<Callback> CallbackPtr;

int run(const Ice::CommunicatorPtr&);

int
main(int argc, char* argv[])
{
    int status = EXIT_SUCCESS;

    try
    {
        //
        // CommunicatorHolder's ctor initializes an Ice communicator,
        // and its dtor destroys this communicator.
        //
        Ice::CommunicatorHolder ich(argc, argv, "config.client");

        //
        // The communicator initialization removes all Ice-related arguments from argc/argv
        //
        if(argc > 1)
        {
            cerr << argv[0] << ": too many arguments" << endl;
            status = EXIT_FAILURE;
        }
        else
        {
            status = run(ich.communicator());
        }
    }
    catch(const std::exception& ex)
    {
        cerr << argv[0] << ": " << ex.what() << endl;
        status = EXIT_FAILURE;
    }

    return status;
}

void menu();

int
run(const Ice::CommunicatorPtr& communicator)
{
    Demo::HelloPrx hello = Demo::HelloPrx::checkedCast(communicator->propertyToProxy("Hello.Proxy"));
    if(!hello)
    {
        cerr << "invalid proxy" << endl;
        return EXIT_FAILURE;
    }

    menu();

    CallbackPtr cb = new Callback();

    char c = 'x';
    do
    {
        try
        {
            cout << "==> ";
            cin >> c;
            if(c == 'i')
            {
                hello->sayHello(0);
            }
            else if(c == 'd')
            {
                hello->begin_sayHello(5000, Demo::newCallback_Hello_sayHello(cb, &Callback::response, &Callback::exception));
            }
            else if(c == 's')
            {
                hello->shutdown();
            }
            else if(c == 'x')
            {
                // Nothing to do
            }
            else if(c == '?')
            {
                menu();
            }
            else
            {
                cout << "unknown command `" << c << "'" << endl;
                menu();
            }
        }
        catch(const Ice::Exception& ex)
        {
            cerr << ex << endl;
        }
    }
    while(cin.good() && c != 'x');

    return EXIT_SUCCESS;
}

void
menu()
{
    cout <<
        "usage:\n"
        "i: send immediate greeting\n"
        "d: send delayed greeting\n"
        "s: shutdown server\n"
        "x: exit\n"
        "?: help\n";
}
