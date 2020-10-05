#ifndef APPSTATE_H
#define APPSTATE_H


class appState
{
    public:
        static appState* myInstance();

        // keep track of whether or not the bot has been initialised, and whether or not it's active.
        bool botInitialised = false; // hardware is ready for commands from the software.
        bool botActive = false; // app is active - no interruptions / breaks yet.

        bool isBotActive();

    protected:

    private:
        appState();
        virtual ~appState();

        // Prohibit external replication constructs
        appState(const appState &state);
        // Prohibit external assignment operations
        const appState &operator=(const appState &state);

};

#endif // APPSTATE_H
