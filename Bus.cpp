#include "Bus.h"

Bus::Bus()
{
  m_sends.SendCount = m_sendsList.size();
  m_sends.pSends = m_sendsList.data();
}
