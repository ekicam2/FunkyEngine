#pragma once

#include "Math/LinearColor.h"
#include "Math/Vector3.h"
#include "Math/Vector2.h"

#include "Core/Containers.h"

#include "BasicTypes.h"

namespace Funky
{
	struct ParticleSystem
	{
		struct ParticleEmitter
		{
			struct Particle
			{
				float Lifetime;
				Math::Vector2f Size = Math::Vector2f(1.0f, 1.0f);
				Math::Vector3f Position = Math::Vector3f(0.0f, 0.0f, 0.0f);
				Math::Vector3f Velocity = Math::Vector3f(0.0f, 1.0f, 0.0f);
				Math::LinearColor Color;

				bool bAlive = true;


				void Update(float DeltaTime)
				{
					if (!bAlive)
						return;

					TimeElapsed += DeltaTime;
					if (TimeElapsed >= Lifetime)
					{
						Die();
						return;
					}

					Position += Velocity * DeltaTime;
				}

				void Reset()
				{
					bAlive = true;
					TimeElapsed = 0.0f;
				}

				void Die()
				{
					bAlive = false;
				}

			private:
				float TimeElapsed;
			};

			enum class ESpawnType
			{
				Deferred,
				Burst
			};

			float LifeTime = 1.0f;
			ESpawnType SpawnType = ESpawnType::Burst;
			u32 Rate;

			darray<Particle> Particles;

			void Update(float DeltaTime)
			{
				TimeElapsed += DeltaTime;

				if (TimeElapsed >= LifeTime)
				{
					TimeElapsed = 0.0f;

					for (auto& Particle : Particles)
					{
						Particle.Position = Math::Vector3f(0.0f, 0.0f, 0.0f);
						Particle.Reset();
					}
				}

				for (auto& Particle : Particles)
				{
					Particle.Update(DeltaTime);
				}
			}

		private:
			float TimeElapsed;

		};

		darray<ParticleEmitter> Emiters;

		void Update(float DeltaTime)
		{
			for (auto& Emiter : Emiters)
			{
				Emiter.Update(DeltaTime);
			}
		}
	};
}