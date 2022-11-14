/*

    Copyright 2019 dYdX Trading Inc.

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.

*/

pragma solidity 0.5.7;
pragma experimental ABIEncoderV2;

import { Ownable } from "openzeppelin-solidity/contracts/ownership/Ownable.sol";
import { IAutoTrader } from "../../protocol/interfaces/IAutoTrader.sol";
import { Account } from "../../protocol/lib/Account.sol";
import { Require } from "../../protocol/lib/Require.sol";
import { Types } from "../../protocol/lib/Types.sol";


/**
 * @title DaiMigrator
 * @author dYdX
 *
 * Allows for moving SAI positions to DAI positions.
 */
contract DaiMigrator is
    Ownable,
    IAutoTrader
{
function coverage_0x4c4443b8(bytes32 c__0x4c4443b8) public pure {}

    using Types for Types.Wei;
    using Types for Types.Par;

    // ============ Constants ============

    bytes32 constant FILE = "DaiMigrator";

    uint256 constant SAI_MARKET = 1;

    uint256 constant DAI_MARKET = 3;

    // ============ Events ============

    event LogMigratorAdded(
        address migrator
    );

    event LogMigratorRemoved(
        address migrator
    );

    // ============ Storage ============

    // the addresses that are able to migrate positions
    mapping (address => bool) public g_migrators;

    // ============ Constructor ============

    constructor (
        address[] memory migrators
    )
        public
    {coverage_0x4c4443b8(0x64a61f914b3695feba3ac8d005a1c33e26d7bfb697d858ffc4d8abc38ead24f1); /* function */ 

coverage_0x4c4443b8(0xe934ca66f61b00742b7c28e5239f4c80cc3568eeffa8b0a22a5ec26840a0820d); /* line */ 
        coverage_0x4c4443b8(0x3e2cca7faf76a51ea311b644029a93c845d005b543bc6e886bfb62218321b33c); /* statement */ 
for (uint256 i = 0; i < migrators.length; i++) {
coverage_0x4c4443b8(0xaa0bfceeeb03a4f7608d23f6acd04b2c568daa413810edfe4a4fb30f051b4edc); /* line */ 
            coverage_0x4c4443b8(0x668c0ad27824b20bae8e700285f82b61060ef147d285a1896c222f1f4a8ed24d); /* statement */ 
g_migrators[migrators[i]] = true;
        }
    }

    // ============ Admin Functions ============

    function addMigrator(
        address migrator
    )
        external
        onlyOwner
    {coverage_0x4c4443b8(0xb01de64b7b26b7bbb88c02dc7c246d580b653e2f8bb81e6d1c0269d9db884ab0); /* function */ 

coverage_0x4c4443b8(0xc76afdf21ba8fd100c2d156c5fdad3c33f26ee8e577a640374814612f163b3af); /* line */ 
        coverage_0x4c4443b8(0x269259cf1bcf1566ed796df9bab7f3a1f1d15fa6396b33e645fc18d24ff025b9); /* statement */ 
emit LogMigratorAdded(migrator);
coverage_0x4c4443b8(0x24a6407bdf4db9783f55d2ebcc658772b397b70885ef5b8c6296076d026584c0); /* line */ 
        coverage_0x4c4443b8(0x2bfa94f0234e1eaf7dacc6a46a05c01dda15460cba8b4d0d538bcd42594c80ce); /* statement */ 
g_migrators[migrator] = true;
    }

    function removeMigrator(
        address migrator
    )
        external
        onlyOwner
    {coverage_0x4c4443b8(0xaa366d2fb12a7d496a47cbfca2625ac0d9f686c2bac38de744b1535c7d2717ce); /* function */ 

coverage_0x4c4443b8(0x1cd73e018645ec439197586f45a08804aed8ac9e3a2991249feed81d35058380); /* line */ 
        coverage_0x4c4443b8(0x44a76a075459d8421440c0cb9e4259343e4b1d985780e09ec27ff7dd36a94941); /* statement */ 
emit LogMigratorRemoved(migrator);
coverage_0x4c4443b8(0xcf2ab9053f279f7d0ec49b1689a8286c1c6490712a99cff17c00f4bc97738a1b); /* line */ 
        coverage_0x4c4443b8(0xca1752ed1ab69825dbdeea5e70dbde09cb048a66644f755de5587424839415ba); /* statement */ 
g_migrators[migrator] = false;
    }

    // ============ Only-Solo Functions ============

    function getTradeCost(
        uint256 inputMarketId,
        uint256 outputMarketId,
        Account.Info memory /* makerAccount */,
        Account.Info memory takerAccount,
        Types.Par memory oldInputPar,
        Types.Par memory newInputPar,
        Types.Wei memory inputWei,
        bytes memory /* data */
    )
        public
        /* view */
        returns (Types.AssetAmount memory)
    {coverage_0x4c4443b8(0xcbfcdec7415c4e2b51138beb45487c9ac6d97c282c4c6bcb04c357df61b73f0a); /* function */ 

coverage_0x4c4443b8(0x565d5b2a500ede2814f8852eaa38aa010ec5f9832b1909308a973e424c21069f); /* line */ 
        coverage_0x4c4443b8(0x1c2e49bcef9f14ae14a30255de845db0a202a6d87717aaadbdb8163e91b716b0); /* statement */ 
Require.that(
            g_migrators[takerAccount.owner],
            FILE,
            "Migrator not approved",
            takerAccount.owner
        );

coverage_0x4c4443b8(0xc96d0b916231d74a8010bf386e0eabfc1605ba0d5e126d1f5ce9e38f4fcbd432); /* line */ 
        coverage_0x4c4443b8(0x55203e0ec5896a823f9ae3a007b439d1c7bb29ae8349407f5a72d7ec41659a32); /* statement */ 
Require.that(
            inputMarketId == SAI_MARKET && outputMarketId == DAI_MARKET,
            FILE,
            "Invalid markets"
        );

        // require that SAI amount is getting smaller (closer to zero)
coverage_0x4c4443b8(0x96377e8841b37f90945cd1f70ca0c2450d1ddf80c1081de6af092c6c860b5ae6); /* line */ 
        coverage_0x4c4443b8(0xc56bafc29f5ad0de7090906646c76aa0b95a6a8def09de5c4f3db7184c9acc33); /* statement */ 
if (oldInputPar.isPositive()) {coverage_0x4c4443b8(0xd0130e4ef8225de5c478945bba6652032d09253fe901ebe21c4dd2e23f329bb6); /* branch */ 

coverage_0x4c4443b8(0x7c7d6e783efa8a3df8aa747e099a756c2bcc10425826649fe88908a3e832c6ae); /* line */ 
            coverage_0x4c4443b8(0xa813c0fda6ccbf688b4461e84f1c2586d76a9de67bb9ff720f80325b8706bbcb); /* statement */ 
Require.that(
                inputWei.isNegative(),
                FILE,
                "inputWei must be negative"
            );
coverage_0x4c4443b8(0xcbcd98473e0403e10586aaac34658dbaf5e1296570e8b4f08fbe65adb0a68da9); /* line */ 
            coverage_0x4c4443b8(0xf9f4870900274d7dfd76dae62b4129fb7e6973fb89ce1da91ea32600a1782eb9); /* statement */ 
Require.that(
                !newInputPar.isNegative(),
                FILE,
                "newInputPar cannot be negative"
            );
        } else {coverage_0x4c4443b8(0xaddba0bddb888a9e57637efe1f2381217f9521fc19a96193b79be3530cec73ab); /* statement */ 
coverage_0x4c4443b8(0x45cee52573f266d3bd72c3778bcc6cdaf8365bc51a4d5a724e1120d902b436b7); /* branch */ 
if (oldInputPar.isNegative()) {coverage_0x4c4443b8(0x232ffcd475f90d6cf6f733d89e62d24729951a2deb75f369ded7324307608fcd); /* branch */ 

coverage_0x4c4443b8(0xc49e9434611b872071d4175f0f92ee6f094b1c1bf4e70384cdf63db3927d2690); /* line */ 
            coverage_0x4c4443b8(0xf29410abaa433c819690f7cb7ed3098b5a9952c63c4ee920b4188a866960289c); /* statement */ 
Require.that(
                inputWei.isPositive(),
                FILE,
                "inputWei must be positive"
            );
coverage_0x4c4443b8(0x55500e0cc12fc667762cafcdb4a9a4d11a5f35becf4ce2ea0ddd0d10ae367a51); /* line */ 
            coverage_0x4c4443b8(0xddc3c6d63a3ef35600ecc41cddd647ff5d770b223f1d96b2992547dd932b40b4); /* statement */ 
Require.that(
                !newInputPar.isPositive(),
                FILE,
                "newInputPar cannot be positive"
            );
        } else {coverage_0x4c4443b8(0xe6113b9c0ba5d58b09c30fb49378e6fa6f498210d19ae61bfc8a19ceaa5e7d1e); /* branch */ 

coverage_0x4c4443b8(0x566ea06d61cdcddaf24b8827a9006060076d41812137c9756738353153cd16cf); /* line */ 
            coverage_0x4c4443b8(0x9c4bb0905766b06e55736f07f7384eed8315839b1b0bfaa8084805738e03caa0); /* statement */ 
Require.that(
                inputWei.isZero() && newInputPar.isZero(),
                FILE,
                "inputWei must be zero"
            );
        }}

        /* return the exact opposite amount of SAI in DAI */
coverage_0x4c4443b8(0x84680661f02412f7b9e00c97d168b7ff5835e683d67af5e49990e0635892edc4); /* line */ 
        coverage_0x4c4443b8(0x8c00451d0dd44cfda091f46cf5df29c6ab3974deb8297aa5dda8ffe349b701c0); /* statement */ 
return Types.AssetAmount ({
            sign: !inputWei.sign,
            denomination: Types.AssetDenomination.Wei,
            ref: Types.AssetReference.Delta,
            value: inputWei.value
        });
    }
}
