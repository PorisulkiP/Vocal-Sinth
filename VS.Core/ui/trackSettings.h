#ifndef TRACKSETTINGS_H
#define TRACKSETTINGS_H

#include <cmath>
#include <vector>
#include <algorithm>

using namespace std;

namespace trackSettings {

    enum PitchPointShape {
        io, // SineInOut
        l, // Linear
        i, // SineIn
        o // SineOut
    };

    class Vibrato {
    public:
        Vibrato(){}
        ~Vibrato(){}
        Vibrato(const Vibrato &ref) {
            _length = ref.length();
            _period = ref.period();
            _depth = ref.depth();
            _shift = ref.shift();
            _drift = ref.drift();
        }

        void setLength(int value) { _length = max(0, min(100, value));    }
        void setPeriod(int value) { _period = max(5, min(500, value));    }
        void setDepth(int value)  { _depth = max(5, min(200, value));     }
        void setShift(int value)  { _shift = max(0, min(100, value));     }
        void setDrift(int value)  { _drift = max(-100, min(100, value));  }
        void setIn(int value){
            _in = max(0, min(100, value));
            _out = min(_out, 100 - _in);
        }
        void setOut(int value){
            _out = max(0, min(100, value));
            _in = min(_in, 100 - _out);
        }

        float length() const    { return _length;   }
        float period() const    { return _period;   }
        float depth()  const    { return _depth;    }
        float shift()  const    { return _shift;    }
        float drift()  const    { return _drift;    }
        float in() const        { return _in;       }
        float out() const       { return _out;      }

    private:
        // Процент вибрато от длины ноты.
        float _length = 0;
        // Период в миллисекундах.
        float _period = 175;
        // Глубина вибрато в центах (1 semitone = 100 cents).
        float _depth = 25;
        // Процент затухания от длины вибрато.
        float _in = 10;
        // Процент затухания от длины вибрато.
        float _out = 10;
        // Процент сдвига от продолжительности периода.
        float _shift;
        float _drift;

        float _normalizedStart = 1. - _length / 100.;
    };

    class PitchPoint {
    public:
        PitchPoint(float x, float y, PitchPointShape shape = io) {
            _X = x;
            _Y = y;
            _shape = shape;
        }
        ~PitchPoint(){}

        void setX(float x) {_X = x;}
        void setY(float y) {_Y = y;}
        void setShape(PitchPointShape shape) { _shape = shape; }

        // Перегружаются операторы необходимые для сортировки точек
        friend constexpr inline bool operator==(const PitchPoint &p1, const PitchPoint &p2) noexcept
        { return p1._X == p2._X; }

        friend constexpr inline bool operator<(const PitchPoint &p1, const PitchPoint &p2) noexcept
        { return p1._X < p2._X; }

        float X() {return _X;}
        float Y() {return _Y;}
        PitchPointShape shape() {return _shape;}

    protected:
        float _X, _Y;
        PitchPointShape _shape;
    };

    class Pitch {
    public:
        Pitch(){}
        ~Pitch(){}

        vector<PitchPoint> *data = new vector<PitchPoint>();
        bool snapFirst = true;

        void AddPoint(PitchPoint p) {
            data->push_back(p);
            sort(data->begin(), data->end());
        }

        void RemovePoint(PitchPoint p) {
            data->erase(find(data->begin(), data->end(), p));
        }

        template<class Project, class Track>
        double sample(Project *project, Track *note, double tick);
    };

} // namespace trackSettings

#endif // TRACKSETTINGS_H
