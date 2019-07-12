class UniqueID
{
    public:
        static unsigned int Get() { return id++; }
    private:
        static unsigned int id;
};
unsigned int UniqueID::id = 0;
