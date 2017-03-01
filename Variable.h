#ifndef VARIABLE_H
#define VARIABLE_H

#include <QList>
#include <QString>
#include <QVariant>

const QVector<QString> analogUnits = {
    /*  0 */ "Unused",
    /*  1 */ "Deg. Celsius",
    /*  2 */ "Deg. Fahrenheit",
    /*  3 */ "Feet per Min.",
    /*  4 */ "Pascals",
    /*  5 */ "KPascals",
    /*  6 */ "lbs/sqr.inch",
    /*  7 */ "Inches of WC",
    /*  8 */ "Watts",
    /*  9 */ "KWatts",
    /* 10 */ "KWHours",
    /* 11 */ "Volts",
    /* 12 */ "KVolts",
    /* 13 */ "Amperes",
    /* 14 */ "Milliamps",
    /* 15 */ "CFM",
    /* 16 */ "Seconds",
    /* 17 */ "Minutes",
    /* 18 */ "Hours",
    /* 19 */ "Days",
    /* 20 */ "Time",
    /* 21 */ "Ohms",
    /* 22 */ "%",
    /* 23 */ "%RH",
    /* 24 */ "p/min",
    /* 25 */ "Counts",
    /* 26 */ "%Open",
    /* 27 */ "%Close",
    /* 28 */ "CFH",
    /* 29 */ "GPM",
    /* 30 */ "GPH",
    /* 31 */ "GAL",
    /* 32 */ "CF",
    /* 33 */ "BTU",
    /* 34 */ "CMH",
    /* 35 */ "Custom1",
    /* 36 */ "Custom2",
    /* 37 */ "Custom3",
    /* 38 */ "Custom4",
    /* 39 */ "Custom5",
    /* 40 */ "Custom6",
    /* 41 */ "Custom7",
    /* 42 */ "Custom8"
};

const QVector<QPair<QString, QString>> digitalUnits = {
    /*  1/12 */ QPair<QString, QString>("Off", "On"),
    /*  2/13 */ QPair<QString, QString>("Close", "Open"),
    /*  3/14 */ QPair<QString, QString>("Stop", "Start"),
    /*  4/15 */ QPair<QString, QString>("Dis", "Enable"),
    /*  5/16 */ QPair<QString, QString>("Norm", "Alarm"),
    /*  6/17 */ QPair<QString, QString>("Norm", "High"),
    /*  7/18 */ QPair<QString, QString>("Norm", "Low"),
    /*  8/19 */ QPair<QString, QString>("No", "Yes"),
    /*  9/20 */ QPair<QString, QString>("Cool", "Heat"),
    /* 10/21 */ QPair<QString, QString>("Un", "Occupied"),
    /* 11/22 */ QPair<QString, QString>("Low", "High")
};

enum VariableUnitType
{
    DIGITAL = 0,
    ANALOG = 1,
};

typedef struct
{
    VariableUnitType type;
} VariableCommonValue;

typedef struct
{
    VariableUnitType type;
    int unit;
    float value;
} VariableAnalogValue;

typedef struct
{
    VariableUnitType type;
    int unit;
    bool value;
    bool inverted;
} VariableDigitalValue;

union VariableValue
{
    VariableCommonValue value;
    VariableAnalogValue analogValue;
    VariableDigitalValue digitalValue;

    VariableValue()
    {
        analogValue.unit = 0;
        analogValue.value = 0;

        digitalValue.inverted = 0;
        digitalValue.unit = 0;
        digitalValue.value = 0;
    }

    bool operator==(const VariableValue& r) const
    {
        if (this->value.type != r.value.type)
        {
            return false;
        }

        if (this->value.type == ANALOG)
        {
            return this->analogValue.unit == r.analogValue.unit;
        }

        if (this->value.type == DIGITAL)
        {
            return this->digitalValue.unit == r.digitalValue.unit;
        }

        return false;
    }

    bool operator<(const VariableValue& r) const
    {
        return false;
    }
};
Q_DECLARE_METATYPE(VariableValue);


typedef struct
{
    QString fullLabel;
    bool auto_;
    VariableValue value;
    QString label;
} Variable;

#endif // VARIABLE_H
