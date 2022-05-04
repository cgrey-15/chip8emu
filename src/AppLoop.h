#ifndef APP_LOOP_H
#define APP_LOOP_H

namespace the {
    class AppLoopBase {
    public:
        AppLoopBase() = default;
        AppLoopBase(AppLoopBase&&)=delete;

        AppLoopBase(const AppLoopBase&) = delete;
        virtual AppLoopBase& operator=(const AppLoopBase&) = delete;

        virtual AppLoopBase& operator=(AppLoopBase&& rhs)=delete;
        virtual ~AppLoopBase();

        virtual int startLoop()=0;
    };
}
#endif // APP_LOOP_H
