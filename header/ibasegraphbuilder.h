#ifndef IBASEGRAPHBUILDER_H
#define IBASEGRAPHBUILDER_H


class IBaseGraphBuilder
{
    protected:
        IBaseGraphBuilder() = default;
        virtual ~IBaseGraphBuilder() = default;

    public:
        IBaseGraphBuilder& operator=(const IBaseGraphBuilder&) = delete;
};

#endif // IBASEGRAPHBUILDER_H
