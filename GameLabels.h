struct GameLabels
{
    size_t count;
    vector<string> names;
    map<Win32Gui::ControlCode, unsigned int> indices;

    GameLabels(const vector<string>& labels) :
        count(labels.size()),
        names(labels)
    {
        for (unsigned int index=0; index<labels.size(); ++index)
        {
            indices[UniqueID::Get()] = index;
        }
    }
    bool HasControlCode(const Win32Gui::ControlCode code) const
    {
        return indices.count(code);
    }
    unsigned int IndexFromCode(const Win32Gui::ControlCode code) const
    {
        assert(HasControlCode(code), "invalid code");
        return indices.at(code);
    }
};
