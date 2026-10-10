#pragma once

#include <TCISDK/internal.h>
#include <TCISDK/sdk/vector3.h>
#include <string>

namespace TCISDK {

class Player {
private:
    int m_id;

public:
    inline explicit Player(int id) : m_id(id) {}

    static Player GetPlayerBySteamId(long long id) noexcept {
        return Player(TCISDK_Internal::GetPlayerBySteamID(id));
    }

    inline int GetId() const noexcept { return m_id; }

    inline Vector3 GetPosition() const noexcept {
        return Vector3(GetPlayerPosition(m_id));
    }

    inline float GetHealth() noexcept {
        return GetPlayerHealth(m_id);
    }

    inline float GetMaxHealth() noexcept {
        return GetPlayerMaxHealth(m_id);
    }

    inline void SetHealth(float value) noexcept {
        SetPlayerHealth(m_id, value);
    }

    inline void SetPosition(const Vector3& pos) noexcept {
        SetPlayerPosition(m_id, pos.ToRaw());
    }

    inline void SendMessage(const std::string& msg) noexcept {
        SendPlayerMessage(m_id, msg.c_str());
    }

    inline void Kill() noexcept {
        KillPlayer(m_id);
    }
};

} // namespace TCISDK