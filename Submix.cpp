#include "Submix.h"

Submix::Submix(unsigned int inputChannels, unsigned int inputSampleRate) 
              : m_inputChannels(inputChannels),
                m_inputSampleRate(inputSampleRate)
{
  m_sends.SendCount = 0;
  m_sends.pSends = m_sendList.data();

  m_fxs.EffectCount = 0;
  m_fxs.pEffectDescriptors = m_fxsList.data();
}

