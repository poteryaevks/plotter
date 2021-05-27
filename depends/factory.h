#ifndef FACTORY_H
#define FACTORY_H

#include <map>
#include <functional>
#include <memory>

#include <boost/format.hpp>

namespace almaz {
namespace misc {
//!
//! \brief The IFactory class - класс маштабируемой фабрики.
//! Если фабрика маштабируема, при добавлении нового
//! класса - наследникав код не нужно вносить никаких изменений
//!
template <class AbstractProduct,
          typename IdentifierType,
          typename CreatorType,
          typename ...Params>
class IFactory
{
private:
    //!
    typedef std::shared_ptr<AbstractProduct> Ptr;
    //! регистрация - это просто добавления указателя
    //! на функцию создания объекта в этот массив:
    typedef std::map<IdentifierType, CreatorType> CallbackMap;
    //
    CallbackMap callbacks_;
public:
    //!
    //! \brief RegisterShape - регистрация фигуры
    //! \param shapeId
    //! \param createFn
    //! \return  Возвращает значение true, если регистрация прошла успешно
    //!
    bool Register(IdentifierType id, CreatorType createFunction){
        return callbacks_.insert(typename CallbackMap::value_type(id, createFunction)).second;
    }
    //!
    //! \brief UnregisteredShape - возвращает признак регистрации фигуры c shapeId
    //! \param shapeId
    //! \return
    //!
    bool Unregistered(IdentifierType id){
        return callbacks_.erase(id) == 1;
    }
    //!
    //! \brief CreateShape - создает фигуру с указанным shapeId
    //! \param shapeId
    //! \return
    //!
    Ptr Create(IdentifierType id, Params... params){
        auto i =  callbacks_.find(id);
        //! Пока только исключение.
        //! Познее стоит добавить обработчик
        if(i == callbacks_.end()){
            throw std::runtime_error((boost::format("Invalid identifier while creating in factory: %d") % id).str());
            return nullptr;
        }
        return (i->second)(params...);
    }
};
}

}
#endif // FACTORY_H
