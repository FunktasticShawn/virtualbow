#pragma once
#include "gui/units/UnitSystem.hpp"
#include <QtWidgets>
#include <limits>

enum class ValueRange {
    UNRESTRICTED,
    POSITIVE,
    NEGATIVE,
    NON_POSITIVE,
    NON_NEGATIVE
};

class DoubleSpinBox: public QDoubleSpinBox {
public:
    DoubleSpinBox(ValueRange range, const UnitGroup& units, QWidget* parent = nullptr)
        : QDoubleSpinBox(parent)
    {
        setDecimals(6);

        switch(range) {
        case ValueRange::UNRESTRICTED:
            setMinimum(std::numeric_limits<double>::lowest());
            setMaximum(std::numeric_limits<double>::max());
            break;

        case ValueRange::POSITIVE:
            setMinimum(std::numeric_limits<double>::min());
            setMaximum(std::numeric_limits<double>::max());
            break;

        case ValueRange::NEGATIVE:
            setMinimum(std::numeric_limits<double>::lowest());
            setMaximum(-std::numeric_limits<double>::min());
            break;

        case ValueRange::NON_POSITIVE:
            setMinimum(std::numeric_limits<double>::lowest());
            setMaximum(0.0);
            break;

        case ValueRange::NON_NEGATIVE:
            setMinimum(0.0);
            setMaximum(std::numeric_limits<double>::max());
            break;
        }

        updateUnit(units.getSelectedUnit());
        QObject::connect(&units, &UnitGroup::selectionChanged, this, [&](const Unit& unit) {
            updateUnit(unit);
        });
    }

    QString textFromValue(double value) const override {
        return QString::number(unit.fromBase(value));
    }

    double valueFromText(const QString& text) const override {
        QString number = text;
        number.remove(suffix());
        return unit.toBase(QLocale().toDouble(number));
    }

    void updateUnit(const Unit& unit) {
        setSuffix(" " + unit.getName());
        setValue(value());    // Update text
    }

private:
    Unit unit;
};
