#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H

#include "../services/InteractionManager.h"
#include "../services/PostManager.h"
#include "../services/SocialManager.h"
#include "../services/UserManager.h"

// Coordinates simple text-file storage without owning any application data.
class FileManager {
public:
    static bool loadAll(UserManager& userManager, PostManager& postManager,
                        SocialManager& socialManager,
                        InteractionManager& interactionManager);

    static bool saveAll(const UserManager& userManager,
                        const PostManager& postManager,
                        const SocialManager& socialManager,
                        const InteractionManager& interactionManager);
};

#endif
