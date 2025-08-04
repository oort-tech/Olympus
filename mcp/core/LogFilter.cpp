#include "LogFilter.hpp"
#include "param.hpp"

namespace mcp
{
    bool LogFilter::isRangeFilter() const
    {
        if (m_addresses.size())
            return false;

        for (auto const& t : m_topics)
        {
            if (t.size())
                return false;
        }

        return true;
    }

    bool LogFilter::matches(log_bloom _bloom) const
    {
        if (m_addresses.size())
        {
            for (auto const& i : m_addresses)
            {
                if (_bloom.containsBloom<3>(dev::sha3(i)))
                    goto OK1;
            }

            return false;
        }
        OK1:
        for (auto const& t : m_topics)
        {
            if (t.size())
            {
                for (auto const& i : t)
                {
                    if (_bloom.containsBloom<3>(dev::sha3(i)))
                        goto OK2;
                }
                return false;
            OK2:;
            }
        }

        return true;
    }

    log_entries LogFilter::matches(dev::eth::TransactionReceipt const& _r, uint64_t const& _mci) const
    {
        // there are no addresses or topics to filter
        if (isRangeFilter())
            return _r.log();

        log_entries ret;
        log_bloom _bloom = _r.bloom();
        //if (!mcp::chainParams()->IsOIP5(_mci))///OIP5
        if (!mcp::param::get()->IsOIP5(_mci))///OIP5
            _bloom = mcp::bloom(_r.log());

        if (matches(_bloom))
        {
            for (log_entry const& e : _r.log())
            {
                if (!m_addresses.empty() && !m_addresses.count(e.address))
                    goto continue2;
                for (unsigned i = 0; i < 4; ++i)
                {
                    if (!m_topics[i].empty() && (e.topics.size() < i || !m_topics[i].count(e.topics[i])))
                        goto continue2;
                }
                ret.push_back(e);
            continue2:;
            }
        }

        return ret;
    }
}
