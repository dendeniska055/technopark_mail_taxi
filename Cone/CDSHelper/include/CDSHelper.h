#pragma once
#include <cds/init.h>
#include <iostream>
namespace Cone {
    namespace CDSHelper {

        class CDSThreadInitializer {
        public:
            CDSThreadInitializer();

            ~CDSThreadInitializer();
        };

        inline CDSThreadInitializer::CDSThreadInitializer() {
            cds::threading::Manager::attachThread();
        }

        inline CDSThreadInitializer::~CDSThreadInitializer() {
            cds::threading::Manager::detachThread();
        }

        template<class GC>
        class CDSInitializer : private boost::noncopyable {
        private:
            std::shared_ptr <GC> gcObj;
        private:
            CDSInitializer();

        public:
            static CDSInitializer &initialize();

            ~CDSInitializer();
        };

        template<class GC>
        CDSInitializer<GC> &CDSInitializer<GC>::initialize() {
            static CDSInitializer<GC> obj;
            return obj;
        }

        template<class GC>
        CDSInitializer<GC>::CDSInitializer() {
            cds::Initialize();
            gcObj = std::make_shared<GC>();
        }

        template<class GC>
        CDSInitializer<GC>::~CDSInitializer() {
            cds::Terminate();
        }
    }
}