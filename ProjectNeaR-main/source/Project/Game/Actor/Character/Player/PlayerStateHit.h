#include "PlayerStateBase.h"
class Actor;
class PlayerStateHit :
    public PlayerStateBase, public std::enable_shared_from_this<PlayerStateHit>
{
public:
    PlayerStateHit(std::weak_ptr<Actor>  player, bool isWait);
    ~PlayerStateHit();
    void Init()override;
    void Update() override;
private:
   
};

