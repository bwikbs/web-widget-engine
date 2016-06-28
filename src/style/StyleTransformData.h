#ifndef __StarFishStyleTransformData__
#define __StarFishStyleTransformData__

#include "style/Style.h"
#include "style/MatrixTransform.h"
#include "style/ScaleTransform.h"
#include "style/RotateTransform.h"
#include "style/SkewTransform.h"
#include "style/TranslateTransform.h"

namespace StarFish {

class ImageData;

class StyleTransformData : public gc {
public:
    enum OperationType {
        Matrix,
        Translate,
        Scale,
        Rotate,
        Skew,
        None
    };

    StyleTransformData()
        : m_type(None)
        , m_matrix(NULL)
        , m_translate(NULL)
        , m_scale(NULL)
        , m_rotate(NULL)
        , m_skew(NULL)
    {
    }

    StyleTransformData(OperationType type)
        : m_type(type)
        , m_matrix(NULL)
        , m_translate(NULL)
        , m_scale(NULL)
        , m_rotate(NULL)
        , m_skew(NULL)
    {
    }

    ~StyleTransformData()
    {
    }

    void setType(OperationType type)
    {
        m_type = type;
    }

    void setMatrix(double a, double b, double c, double d, double e, double f)
    {
        STARFISH_ASSERT(m_type == Matrix);
        if (m_matrix == NULL) {
            m_matrix = new MatrixTransform(a, b, c, d, e, f);
        } else {
            m_matrix->setData(a, b, c, d, e, f);
        }
    }

    void setScale(double a, double b)
    {
        STARFISH_ASSERT(m_type == Scale);
        if (m_scale == NULL) {
            m_scale = new ScaleTransform(a, b);
        } else {
            m_scale->setData(a, b);
        }
    }

    void setRotate(double ang)
    {
        STARFISH_ASSERT(m_type == Rotate);
        if (m_rotate == NULL) {
            m_rotate = new RotateTransform(ang);
        } else {
            m_rotate->setData(ang);
        }
    }

    void setSkew(double angX, double angY)
    {
        STARFISH_ASSERT(m_type == Skew);
        if (m_skew == NULL) {
            m_skew = new SkewTransform(angX, angY);
        } else {
            m_skew->setData(angX, angY);
        }
    }

    void setTranslate(Length x, Length y)
    {
        STARFISH_ASSERT(m_type == Translate);
        if (m_translate == NULL) {
            m_translate = new TranslateTransform(x, y);
        } else {
            m_translate->setData(x, y);
        }
    }

    MatrixTransform* matrix()
    {
        STARFISH_ASSERT(type() == OperationType::Matrix);
        return m_matrix;
    }

    TranslateTransform* translate()
    {
        STARFISH_ASSERT(type() == OperationType::Translate);
        return m_translate;
    }

    ScaleTransform* scale()
    {
        STARFISH_ASSERT(type() == OperationType::Scale);
        return m_scale;
    }

    RotateTransform* rotate()
    {
        STARFISH_ASSERT(type() == OperationType::Rotate);
        return m_rotate;
    }

    SkewTransform* skew()
    {
        STARFISH_ASSERT(type() == OperationType::Skew);
        return m_skew;
    }

    OperationType type() const
    {
        return m_type;
    }

    String* dumpString()
    {
        char temp[100];
        if (m_type == Matrix) {
            snprintf(temp, sizeof temp, "matrix(%.3f %.3f %.3f %.3f %.3f %.3f) ", m_matrix->a(), m_matrix->b(), m_matrix->c(), m_matrix->d(), m_matrix->e(), m_matrix->f());
        } else if (m_type == Scale) {
            snprintf(temp, sizeof temp, "scale(%.3f %.3f) ", m_scale->x(), m_scale->y());
        } else if (m_type == Rotate) {
            snprintf(temp, sizeof temp, "rotate(%.3f) ", m_rotate->angle());
        } else if (m_type == Skew) {
            snprintf(temp, sizeof temp, "skew(%.3f %.3f) ", m_skew->angleX(), m_skew->angleY());
        } else if (m_type == Translate) {
            snprintf(temp, sizeof temp, "translate(%s %s) ", m_translate->tx().dumpString()->utf8Data(), m_translate->ty().dumpString()->utf8Data());
        }
        // TODO
        return String::fromUTF8(temp);
    }

    void changeToFixedIfNeeded(Length fontSize, Font* font)
    {
        STARFISH_ASSERT(type() == OperationType::Translate);
        Length x = m_translate->tx();
        Length y = m_translate->ty();
        x.changeToFixedIfNeeded(fontSize, font);
        y.changeToFixedIfNeeded(fontSize, font);
        m_translate->setData(x, y);
    }

private:
    friend inline bool operator==(const StyleTransformData& a, const StyleTransformData& b);
    friend inline bool operator!=(const StyleTransformData& a, const StyleTransformData& b);

    OperationType m_type;
    // TODO save pointers in union
    MatrixTransform* m_matrix;
    TranslateTransform* m_translate;
    ScaleTransform* m_scale;
    RotateTransform* m_rotate;
    SkewTransform* m_skew;
};

bool operator==(const StyleTransformData& a, const StyleTransformData& b)
{
    if (a.type() != b.type())
        return false;

    switch (a.m_type) {
    case StyleTransformData::OperationType::Matrix:
        if (*(a.m_matrix) != *(b.m_matrix))
            return false;
        break;
    case StyleTransformData::OperationType::Scale:
        if (*(a.m_scale) != *(b.m_scale))
            return false;
        break;
    case StyleTransformData::OperationType::Translate:
        if (*(a.m_translate) != *(b.m_translate))
            return false;
        break;
    case StyleTransformData::OperationType::Rotate:
        if (*(a.m_rotate) != *(b.m_rotate))
            return false;
        break;
    case StyleTransformData::OperationType::Skew:
        if (*(a.m_skew) != *(b.m_skew))
            return false;
        break;
    case StyleTransformData::OperationType::None:
    default:
        break;
    }
    return true;
}

bool operator!=(const StyleTransformData& a, const StyleTransformData& b)
{
    return !operator==(a, b);
}

class StyleTransformDataGroup : public gc {
public:
    StyleTransformDataGroup()
    {
    }

    ~StyleTransformDataGroup()
    {
    }

    void append(StyleTransformData f)
    {
        m_group.push_back(f);
    }

    StyleTransformData at(int i) const
    {
        return m_group[i];
    }

    size_t size() const
    {
        return m_group.size();
    }

    String* dumpString()
    {
        String* str = String::emptyString;
        for (size_t i = 0; i < size(); i++) {
            str = str->concat(at(i).dumpString());
        }
        return str;
    }

private:
    friend inline bool operator==(const StyleTransformDataGroup& a, const StyleTransformDataGroup& b);
    friend inline bool operator!=(const StyleTransformDataGroup& a, const StyleTransformDataGroup& b);

    std::vector<StyleTransformData, gc_allocator<StyleTransformData> > m_group;
};

bool operator==(const StyleTransformDataGroup& a, const StyleTransformDataGroup& b)
{
    if (a.size() != b.size())
        return false;

    for (size_t i = 0; i < a.size(); i++) {
        if (a.at(i) != b.at(i))
            return false;
    }

    return true;
}

bool operator!=(const StyleTransformDataGroup& a, const StyleTransformDataGroup& b)
{
    return !operator==(a, b);
}

}

#endif
