#ifndef __StarFishScriptBindingInstance__
#define __StarFishScriptBindingInstance__


namespace StarFish {

class ScriptBindingInstance : public gc {
public:
    ScriptBindingInstance();
    void initBinding(StarFish* window);
    void* data()
    {
        return m_data;
    }
    void evaluate(String* str);
protected:
    void* m_data;
};

}

#endif
