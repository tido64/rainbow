// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef SCRIPT_WORLD_H_
#define SCRIPT_WORLD_H_

#include <vector>

#include "Common/TypeInfo.h"
#include "Script/Actor.h"

namespace rainbow
{
    class World
    {
    public:
        /// <summary>Clears all actors.</summary>
        void clear() { entities_.clear(); }

        /// <summary>
        ///   Performs <paramref name="action"/> on all actors of type
        ///   <typeparamref name="T"/>.
        /// </summary>
        template <typename T, typename F>
        void for_each(F&& action)
        {
            for (auto&& entity : entities_)
            {
                if (entity.type == type_id<T>())
                    action(static_cast<T*>(entity.actor.get()));
            }
        }

        /// <summary>Removes <paramref name="actor"/>.</summary>
        void remove(Actor& actor)
        {
            for (auto i = entities_.begin(); i != entities_.end(); ++i)
            {
                if (i->actor.get() == &actor)
                {
                    entities_.erase(i);
                    break;
                }
            }
        }

        /// <summary>
        ///   Spawns an actor of type <typeparamref name="T"/> at specified
        ///   location.
        /// </summary>
        /// <returns>Actor that just spawned.</returns>
        template <typename T, typename... Args>
        auto spawn(const Vec2f& position, Args&&... args)
        {
            static_assert(std::is_base_of<Actor, T>::value,
                          "Entity type must be derived from Actor.");

            auto actor = new T();
            entities_.emplace_back(actor);

            actor->initialize(std::forward<Args>(args)...);
            actor->set_position(position);
            return actor;
        }

        void update(uint64_t dt)
        {
            for (auto&& entity : entities_)
                entity.actor->update(dt);
        }

    private:
        struct Entity
        {
            type_id_t type;
            std::unique_ptr<Actor> actor;

            template <typename T>
            Entity(T* actor_) : type(type_id<T>()), actor(actor_) {}

            Entity(Entity&&) = default;

            ~Entity() { actor->dispose(); }

            auto operator=(Entity&&) -> Entity& = default;
        };

        std::vector<Entity> entities_;
    };
}

#endif
