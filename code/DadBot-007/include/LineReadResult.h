#ifndef LINEREADRESULT_H
#define LINEREADRESULT_H


class LineReadResult
{
    public:
        LineReadResult();
        int analogReadResult[5] = { 0 };
        int calibratedReadResult[5] = { 0 };
        int lineReadResult[5] = { 0 };
        int last_value = 0;

    protected:

    private:
};

#endif // LINEREADRESULT_H
