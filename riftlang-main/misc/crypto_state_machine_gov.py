import os
import hashlib
import base64

# === ENTROPY HARMONIZER (Fallback KDF) ===
def fallback_kdf(input_data, salt, rounds=10000):
    data = input_data.encode() + salt
    for _ in range(rounds):
        data = hashlib.sha256(data).digest()
    return data

# === AURA SEAL GENERATION ===
def generate_aura_seal(plaintext, salt=None, time_cost=4, memory_cost=65536, parallelism=2, metadata=None):
    if salt is None:
        salt = os.urandom(16)
    associated_context = str(metadata).encode() if metadata else b"default_policy"
    try:
        full_input = plaintext + associated_context.decode()
        derived_key = fallback_kdf(full_input, salt)
        aura_seal = hashlib.sha256(derived_key).hexdigest()
        return {
            'derived_key': base64.b64encode(derived_key).decode(),
            'aura_seal': aura_seal,
            'salt': salt.hex(),
            'metadata': metadata,
            'params': {
                'simulated_time_cost': time_cost,
                'simulated_memory_cost': memory_cost,
                'simulated_parallelism': parallelism
            }
        }
    except Exception as e:
        raise RuntimeError(f"Aura harmonization failed: {str(e)}")

# === AURA SEAL VERIFICATION ===
def verify_aura_seal(plaintext, aura_seal, salt_hex, metadata=None):
    try:
        salt = bytes.fromhex(salt_hex)
        associated_context = str(metadata).encode() if metadata else b"default_policy"
        full_input = plaintext + associated_context.decode()
        derived_key = fallback_kdf(full_input, salt)
        recalculated_seal = hashlib.sha256(derived_key).hexdigest()
        return recalculated_seal == aura_seal
    except Exception as e:
        raise RuntimeError(f"Aura verification failed: {str(e)}")

# === POLICY-DRIVEN CRYPTIC STATE MACHINE ===
class CrypticStateMachine:
    def __init__(self, initial_state, policy):
        self.state = initial_state
        self.policy = policy
        self.transitions = {
            'idle': {
                'trigger': 'start',
                'next': 'processing'
            },
            'processing': {
                'trigger': 'seal',
                'next': 'sealed'
            },
            'sealed': {
                'trigger': 'verify',
                'next': 'verified'
            }
        }

    def transition(self, action):
        if self.state in self.transitions:
            transition_rule = self.transitions[self.state]
            if transition_rule['trigger'] == action:
                if self.policy_allows(self.state, action):
                    self.state = transition_rule['next']
                    print(f"✅ Transitioned to '{self.state}'")
                else:
                    print(f"❌ Policy rejected transition from '{self.state}' with action '{action}'")
            else:
                print(f"❌ Invalid action '{action}' for state '{self.state}'")
        else:
            print(f"❌ No transition rule for state '{self.state}'")

    def policy_allows(self, state, action):
        return self.policy.get(state, {}).get('allowed', []) and action in self.policy[state]['allowed']

# === TEST CASES ===
if __name__ == "__main__":
    schema_metadata = {
        "schema_version": "1.0",
        "policy_id": "auth-entropy-v9",
        "entropy_mask": "high",
        "data_class": "sensitive"
    }

    seal = generate_aura_seal("classified_string", metadata=schema_metadata)
    print("Aura Seal Output:")
    print(seal)
    is_valid = verify_aura_seal("classified_string", seal['aura_seal'], seal['salt'], metadata=schema_metadata)
    print("Verification result:", "✔️ Valid" if is_valid else "❌ Invalid")

    # === STATE MACHINE TEST ===
    print("\n=== Cryptic State Machine Test ===")
    policy_rules = {
        'idle': {'allowed': ['start']},
        'processing': {'allowed': ['seal']},
        'sealed': {'allowed': ['verify']},
        'verified': {'allowed': []}
    }

    csm = CrypticStateMachine(initial_state='idle', policy=policy_rules)
    csm.transition('start')
    csm.transition('seal')
    csm.transition('verify')
    csm.transition('hack')  # Invalid action
