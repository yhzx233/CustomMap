// This Header is auto generated by BDSLiteLoader Toolchain
#pragma once
#define AUTO_GENERATED
#include "../Global.h"

#define BEFORE_EXTRA
// Include Headers or Declare Types Here

#undef BEFORE_EXTRA

class ContainerModel {

#define AFTER_EXTRA
// Add Member There

#undef AFTER_EXTRA

#ifndef DISABLE_CONSTRUCTOR_PREVENTION_CONTAINERMODEL
public:
    class ContainerModel& operator=(class ContainerModel const &) = delete;
    ContainerModel(class ContainerModel const &) = delete;
    ContainerModel() = delete;
#endif

public:
    /*0*/ virtual void containerContentChanged(int);
    /*1*/ virtual ~ContainerModel();
    /*2*/ virtual void postInit();
    /*3*/ virtual void releaseResources();
    /*4*/ virtual int getContainerSize() const;
    /*5*/ virtual int getFilteredContainerSize() const;
    /*6*/ virtual void __unk_vfn_6();
    /*7*/ virtual class ContainerWeakRef getContainerWeakRef() const;
    /*8*/ virtual class ItemStack const & getItemStack(int) const;
    /*9*/ virtual std::vector<class ItemStack> const & getItems() const;
    /*10*/ virtual class ItemInstance const & getItemInstance(int) const;
    /*11*/ virtual class ItemStackBase const & getItemStackBase(int) const;
    /*12*/ virtual void __unk_vfn_12();
    /*13*/ virtual void setItem(int, class ItemStack const &);
    /*14*/ virtual bool isValid();
    /*15*/ virtual void __unk_vfn_15();
    /*16*/ virtual void __unk_vfn_16();
    /*17*/ virtual void __unk_vfn_17();
    /*18*/ virtual std::string const & getItemGroupName(int) const;
    /*19*/ virtual void __unk_vfn_19();
    /*20*/ virtual class Container * _getContainer() const;
    /*21*/ virtual int _getContainerOffset() const;
    /*22*/ virtual void _onItemChanged(int, class ItemStack const &, class ItemStack const &);
    /*
    inline bool isItemFiltered(class ItemStackBase const & a0) const{
        bool (ContainerModel::*rv)(class ItemStackBase const &) const;
        *((void**)&rv) = dlsym("?isItemFiltered@ContainerModel@@UEBA_NAEBVItemStackBase@@@Z");
        return (this->*rv)(std::forward<class ItemStackBase const &>(a0));
    }
    inline bool isExpanableItemFiltered(int a0) const{
        bool (ContainerModel::*rv)(int) const;
        *((void**)&rv) = dlsym("?isExpanableItemFiltered@ContainerModel@@UEBA_NH@Z");
        return (this->*rv)(std::forward<int>(a0));
    }
    inline bool isItemInstanceBased() const{
        bool (ContainerModel::*rv)() const;
        *((void**)&rv) = dlsym("?isItemInstanceBased@ContainerModel@@UEBA_NXZ");
        return (this->*rv)();
    }
    inline class Container * _getContainer() const{
        class Container * (ContainerModel::*rv)() const;
        *((void**)&rv) = dlsym("?_getContainer@ContainerModel@@MEBAPEAVContainer@@XZ");
        return (this->*rv)();
    }
    inline enum ContainerExpandStatus getItemExpandStatus(int a0) const{
        enum ContainerExpandStatus (ContainerModel::*rv)(int) const;
        *((void**)&rv) = dlsym("?getItemExpandStatus@ContainerModel@@UEBA?AW4ContainerExpandStatus@@H@Z");
        return (this->*rv)(std::forward<int>(a0));
    }
    inline void switchItemExpando(int a0){
        void (ContainerModel::*rv)(int);
        *((void**)&rv) = dlsym("?switchItemExpando@ContainerModel@@UEAAXH@Z");
        return (this->*rv)(std::forward<int>(a0));
    }
    inline void tick(int a0){
        void (ContainerModel::*rv)(int);
        *((void**)&rv) = dlsym("?tick@ContainerModel@@UEAAXH@Z");
        return (this->*rv)(std::forward<int>(a0));
    }
    inline  ~ContainerModel(){
         (ContainerModel::*rv)();
        *((void**)&rv) = dlsym("??1ContainerModel@@UEAA@XZ");
        return (this->*rv)();
    }
    */
    MCAPI ContainerModel(enum ContainerEnumName, int, enum ContainerCategory, bool);
    MCAPI bool isContainerSlotInRange(int) const;
    MCAPI void networkUpdateItem(int, class ItemStack const &, class ItemStack const &);
    MCAPI void registerOnContainerChangedCallback(class std::function<void (int, class ItemStack const &, class ItemStack const &)>);
    MCAPI void registerPlayerNotificationCallback(class std::function<void (int, class ItemStack const &, class ItemStack const &)>);

protected:
    MCAPI void _init();

private:
    MCAPI void _onClientUIItemNetworkChanged(int, class ItemStack const &, class ItemStack const &);

};