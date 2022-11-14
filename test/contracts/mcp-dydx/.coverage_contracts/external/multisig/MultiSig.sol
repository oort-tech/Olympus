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


/**
 * @title MultiSig
 * @author dYdX
 *
 * Multi-Signature Wallet.
 * Allows multiple parties to agree on transactions before execution.
 * Adapted from Stefan George's MultiSigWallet contract.
 *
 * Logic Changes:
 *  - Removed the fallback function
 *  - Ensure newOwner is notNull
 *
 * Syntax Changes:
 *  - Update Solidity syntax for 0.5.X: use `emit` keyword (events), use `view` keyword (functions)
 *  - Add braces to all `if` and `for` statements
 *  - Remove named return variables
 *  - Add space before and after comparison operators
 *  - Add ADDRESS_ZERO as a constant
 *  - uint => uint256
 *  - external_call => externalCall
 */
contract MultiSig {
function coverage_0xc83ff638(bytes32 c__0xc83ff638) public pure {}


    // ============ Events ============

    event Confirmation(address indexed sender, uint256 indexed transactionId);
    event Revocation(address indexed sender, uint256 indexed transactionId);
    event Submission(uint256 indexed transactionId);
    event Execution(uint256 indexed transactionId);
    event ExecutionFailure(uint256 indexed transactionId);
    event OwnerAddition(address indexed owner);
    event OwnerRemoval(address indexed owner);
    event RequirementChange(uint256 required);

    // ============ Constants ============

    uint256 constant public MAX_OWNER_COUNT = 50;
    address constant ADDRESS_ZERO = address(0x0);

    // ============ Storage ============

    mapping (uint256 => Transaction) public transactions;
    mapping (uint256 => mapping (address => bool)) public confirmations;
    mapping (address => bool) public isOwner;
    address[] public owners;
    uint256 public required;
    uint256 public transactionCount;

    // ============ Structs ============

    struct Transaction {
        address destination;
        uint256 value;
        bytes data;
        bool executed;
    }

    // ============ Modifiers ============

    modifier onlyWallet() {coverage_0xc83ff638(0x6de4c6ce462f7b77b8f4ba2722c9c0b363ba546b872e65cc6fcaeef5477f8983); /* function */ 

        /* solium-disable-next-line error-reason */
coverage_0xc83ff638(0xbd8fc6ad48d8e06a8301f0842274fa0c9bebb305becd29537bfe8b1c3cdd1ac4); /* line */ 
        coverage_0xc83ff638(0xe110cee7b5b2cc57e8fb7b2bbaf1e94afaf02fc919faa12ae0cd55d8a03375ee); /* assertPre */ 
coverage_0xc83ff638(0x6936013ccb52776fb0c27d498031144caa9a13e6b34276853026ab0f4c0e8076); /* statement */ 
require(msg.sender == address(this));coverage_0xc83ff638(0x26e28c8a4d2845c10a81562774e9160e8e88440763c17ffd552271677aadb161); /* assertPost */ 

coverage_0xc83ff638(0xd0399304369ef4bf1bf32e96a24e042c77126770135f086c9874a37c7a149532); /* line */ 
        _;
    }

    modifier ownerDoesNotExist(
        address owner
    ) {coverage_0xc83ff638(0xd5c8ce9a3722d3ed072548cd65a4871f4ffccf940f2832a48da291a78573bf24); /* function */ 

        /* solium-disable-next-line error-reason */
coverage_0xc83ff638(0x274837f96c995648bb3e689357c5757a62dc0c9fdcd3bb030d961f78a8ae8154); /* line */ 
        coverage_0xc83ff638(0x9c2cc748710fcbc72891935303c8651252b6ab994e8903654d7463b84add5b43); /* assertPre */ 
coverage_0xc83ff638(0x7dea27a24e5be8a2682b9bfbc4c49a0d773f06c2eb083441695f8dca38248826); /* statement */ 
require(!isOwner[owner]);coverage_0xc83ff638(0xf58392696626ca607bdc028be982fb2588c176df5b1ec7652898f196bee57092); /* assertPost */ 

coverage_0xc83ff638(0x09da639ea54d9a4bc7f8a7ed44b29dc501f35f0cb9548734c6adcb00f52087d7); /* line */ 
        _;
    }

    modifier ownerExists(
        address owner
    ) {coverage_0xc83ff638(0x56b1092d48fc9bda9054de2eacc401931d5f5595cd4c31f67c2993594b348ed9); /* function */ 

        /* solium-disable-next-line error-reason */
coverage_0xc83ff638(0xaeabb5a24c2fdf0f9b65e0317ef9e9c4c0525ba43384ba9dc2669fdf5ac5905f); /* line */ 
        coverage_0xc83ff638(0x7ae915f0e1023b29bb53cfdb0729d50532a563cc67cfdb744cae8d41d7c67601); /* assertPre */ 
coverage_0xc83ff638(0x8eece384bea726a3fc076eabab5ae5cb045a5e6ed984213380997d752aa9aad3); /* statement */ 
require(isOwner[owner]);coverage_0xc83ff638(0xa58fdd4a571609b2f469e666ebacb055813921188d6893e624fb1c5619c7cc82); /* assertPost */ 

coverage_0xc83ff638(0xad272c1986efde60a5df90f6a8d281f7003e7de75f7c63942c334773b6422751); /* line */ 
        _;
    }

    modifier transactionExists(
        uint256 transactionId
    ) {coverage_0xc83ff638(0x0f9be0c851049efbbe7239c3185b1ead7aa845b02aaa6cda217946371ada072d); /* function */ 

        /* solium-disable-next-line error-reason */
coverage_0xc83ff638(0x29598cc7bda78f6740a0b0b5c14ee45fc31debd170f39fe5409734bdc3c4c31c); /* line */ 
        coverage_0xc83ff638(0x87c1e5fb249033e22374e56467a40cfa1f0f7144e0da0eb6296b337aab5fbfb6); /* assertPre */ 
coverage_0xc83ff638(0x8f0b75e5733d5f8cc572218cbb9c5aba986b3195c297dab3c159466c37c83ef8); /* statement */ 
require(transactions[transactionId].destination != ADDRESS_ZERO);coverage_0xc83ff638(0x8d1363cccf5642a38ef9d0f9f23bd12eaef8d14a1f3e395c396fc450d5b5cb38); /* assertPost */ 

coverage_0xc83ff638(0xb6eeb2eab82e6453f08b153bbf01f0274091788c5f884c9451f736f157022e31); /* line */ 
        _;
    }

    modifier confirmed(
        uint256 transactionId,
        address owner
    ) {coverage_0xc83ff638(0x5ca1f6849012de3fb3f76ba4d2f3008fbf4ff714578be5cb11ed352d080259b8); /* function */ 

        /* solium-disable-next-line error-reason */
coverage_0xc83ff638(0xd5c6f3554a2537445b0663137879e05c39ba6f98fbe2190890444090e719ef73); /* line */ 
        coverage_0xc83ff638(0x7a7c402bb276f2b280fee1a0fd181fc5d7bae9f92832e5429852107cdc2faf94); /* assertPre */ 
coverage_0xc83ff638(0x674e1a33092e3d50b26f7f2a6cbb0b42def1fbf5b99dd9f963da3e980d556230); /* statement */ 
require(confirmations[transactionId][owner]);coverage_0xc83ff638(0x0390b5c89ab01db05f922c545fddec93ef76819034e9c4551992b9daeed1dff5); /* assertPost */ 

coverage_0xc83ff638(0x4c2eafef3b7da0ed2cce08c4b8d443886d477c6f14fd276049fc16b64c742c56); /* line */ 
        _;
    }

    modifier notConfirmed(
        uint256 transactionId,
        address owner
    ) {coverage_0xc83ff638(0x0ee8c3528c0faee423c8cf5852cf759b2433d187f05803229ee41ed11b4e5ec7); /* function */ 

        /* solium-disable-next-line error-reason */
coverage_0xc83ff638(0xbbf048ff31f7a466f3c866449b0d2af61a3e55229ae25c86b45baf68e087be6c); /* line */ 
        coverage_0xc83ff638(0xe6d28ccfa5211f84d5b4a7dd5f32fc1e3b7fd5e13ca4c9b528811e0fba20d53c); /* assertPre */ 
coverage_0xc83ff638(0xfc7d02603bcffa1f89427c45cced047042603a50c3eb1aaaef996abfe04a88d2); /* statement */ 
require(!confirmations[transactionId][owner]);coverage_0xc83ff638(0x6f80a07f6a314e95197815e69828ddfff9eca84eb8ed3cf4644346faaa5cc7ba); /* assertPost */ 

coverage_0xc83ff638(0x11631f48173b82e4cb5ddc2972f26166c27125cd9bd511b01f5de02bf512ed20); /* line */ 
        _;
    }

    modifier notExecuted(
        uint256 transactionId
    ) {coverage_0xc83ff638(0x118c96eaac51eb6a951ca6af8fb56d497aa504aa4182e29f9e7e4d584770b0f0); /* function */ 

        /* solium-disable-next-line error-reason */
coverage_0xc83ff638(0xc28577ed4c99d3ee69e85635be54dfd08d07d6efe66f7d2430d9aadce52d305f); /* line */ 
        coverage_0xc83ff638(0x170959a9170ad580ca74846ea888fc5505e83c2c7453bbcc5659f5cdbbcf4a2c); /* assertPre */ 
coverage_0xc83ff638(0x38555bfde83ad6f77835a5ca57ad7d146c0412298ca6bfcd137571c8abbd8ece); /* statement */ 
require(!transactions[transactionId].executed);coverage_0xc83ff638(0x2a5e59a84b271bde3c7c5f0dc5ce02d6d24df94d9c0c67b9ff50f0c1039765bb); /* assertPost */ 

coverage_0xc83ff638(0xccb1d2fa6782f548f0fb9b43738f2de43a11f7bcefa3c2667a2b5ff554704a07); /* line */ 
        _;
    }

    modifier notNull(
        address _address
    ) {coverage_0xc83ff638(0x033806cbe6dfd95e11a9112f34f9b54a28ef222475530b4806e8db7d99152c3c); /* function */ 

        /* solium-disable-next-line error-reason */
coverage_0xc83ff638(0x5d48f4e74b9d80b19c6b403a4e2d58d366d660e4db3596443aa78e5a8d780a98); /* line */ 
        coverage_0xc83ff638(0x1abe0082fabb3b3efc3a5b53a19dbe40f88b9b99bb6943e3dea51ff8622b7789); /* assertPre */ 
coverage_0xc83ff638(0x1646b5b7391aed1f5a694f200b0f54a7950c267008586c3149bee41fbc4cb389); /* statement */ 
require(_address != ADDRESS_ZERO);coverage_0xc83ff638(0x7142daaaa05f52afc062540730a9e5f4d382d1d5d5c30ab8c4e789c7cf64ec51); /* assertPost */ 

coverage_0xc83ff638(0x219e71ea8f9b046063c49dee6bb0e801cb0cf8ce358724d409ca382cbb82c2cd); /* line */ 
        _;
    }

    modifier validRequirement(
        uint256 ownerCount,
        uint256 _required
    ) {coverage_0xc83ff638(0xff2d9e40434beab85fe9eea115796ba5e4ecce1ee68edfee3393dcbc0664441c); /* function */ 

        /* solium-disable-next-line error-reason */
coverage_0xc83ff638(0x51e33b55278402bae13a5bdb82727e4f8487de83f0730df129a3acfc94e24a43); /* line */ 
        coverage_0xc83ff638(0xc8f6b0e0c734686154e4ccb0610a426986608f2cc155062d8f6a611b4e748014); /* assertPre */ 
coverage_0xc83ff638(0x803d45839ee7b7832d2d23b63ac0aabc45e04e62cc4a6339fb77851570c8bab0); /* statement */ 
require(
            ownerCount <= MAX_OWNER_COUNT
            && _required <= ownerCount
            && _required != 0
            && ownerCount != 0
        );coverage_0xc83ff638(0x451959580bba3abec13b29131e9bd14effcfb961fb1f4aebf29169425718e049); /* assertPost */ 

coverage_0xc83ff638(0xfeede3c81dca2fb9c3d9392ae916e48106a5e34cdeacaee9e9da0d12a315b846); /* line */ 
        _;
    }

    // ============ Constructor ============

    /**
     * Contract constructor sets initial owners and required number of confirmations.
     *
     * @param  _owners    List of initial owners.
     * @param  _required  Number of required confirmations.
     */
    constructor(
        address[] memory _owners,
        uint256 _required
    )
        public
        validRequirement(_owners.length, _required)
    {coverage_0xc83ff638(0x62b28b75f449b5c443886076e8722f7b585259b9d1bdfcf9d8316b7c29eb25b7); /* function */ 

coverage_0xc83ff638(0xd103aca9361cb2aa2986260547d5d77e270185f2fe3ed41f495f5d738ca79b40); /* line */ 
        coverage_0xc83ff638(0x39dc751c1f4f8cd3138cb4fa6c114b46618f7e4a79e732997f4928db4d063e3f); /* statement */ 
for (uint256 i = 0; i < _owners.length; i++) {
            /* solium-disable-next-line error-reason */
coverage_0xc83ff638(0x4675546dee94c4fa57315538acb86d1c405ea332f1df70cd3e8e1a47c8a54945); /* line */ 
            coverage_0xc83ff638(0x16ab296403f118b928709449906ba40989f4f4e41503d01e5fb7c5c2495e9746); /* assertPre */ 
coverage_0xc83ff638(0x65742a3fb57fed8a4aed1dfb3bb0974579cc5621b0dd7ae01fea2a975ce9b2a2); /* statement */ 
require(!isOwner[_owners[i]] && _owners[i] != ADDRESS_ZERO);coverage_0xc83ff638(0x5583e1a97974447dc6d75ba5613646b92a7096b59eb7bda5777951277ed7a846); /* assertPost */ 

coverage_0xc83ff638(0xd03749c37d4f3c4a99645f434ed92bb317f3af3320e84d73b026e75961f319cd); /* line */ 
            coverage_0xc83ff638(0xdcaff4081bc2eae839b738aa1faf49bfbf6e3dde6128a689c2d64f7c6cb12fed); /* statement */ 
isOwner[_owners[i]] = true;
        }
coverage_0xc83ff638(0x8ba97fbd28188f8245ade179ea1b090d74a29f50ac57c2909af4c36cbe5e843a); /* line */ 
        coverage_0xc83ff638(0xe3b02df3f0b494563f31ae42427fbd6e343a564c504f8f456e88ead2f9d07b2c); /* statement */ 
owners = _owners;
coverage_0xc83ff638(0xb20e6a3d0bf21c2a56f7019673d502338a3e225a0f439d6d52d9cd8344baa6f5); /* line */ 
        coverage_0xc83ff638(0x7b97574735977ac702d79ad842fd86bbe5192b120354ad4309581b4e87f80e82); /* statement */ 
required = _required;
    }

    // ============ Wallet-Only Functions ============

    /**
     * Allows to add a new owner. Transaction has to be sent by wallet.
     *
     * @param  owner  Address of new owner.
     */
    function addOwner(
        address owner
    )
        public
        onlyWallet
        ownerDoesNotExist(owner)
        notNull(owner)
        validRequirement(owners.length + 1, required)
    {coverage_0xc83ff638(0x52f5ee5863b7273c5c8b9545e000f83ee040384e4f93139efc347195852cf568); /* function */ 

coverage_0xc83ff638(0x32892c4ca7255d270e21df48fabc9c28b02d983fa8f3f39ba7a0c7112613d4ee); /* line */ 
        coverage_0xc83ff638(0x44fc2158774584203b7066d36b2133e6b04128e8cf4bd462c20b9b2639945751); /* statement */ 
isOwner[owner] = true;
coverage_0xc83ff638(0xb4f6c307e36c549a678833846c7d41b3f02f4f833b2eb7bded3b8fe8cffb1130); /* line */ 
        coverage_0xc83ff638(0x11f2ee20caceff1031f01b98a1ec1f89c906df7b0096897b067898dad8eadfd4); /* statement */ 
owners.push(owner);
coverage_0xc83ff638(0x618af0351b35fa36a09bef6da3c54aeec00adce0ca2a62f834b9688636790ef3); /* line */ 
        coverage_0xc83ff638(0x39ff8fefd6edcbfc386da5c73ab0a622258f5e8f66bd3321598ca5f1a2440fde); /* statement */ 
emit OwnerAddition(owner);
    }

    /**
     * Allows to remove an owner. Transaction has to be sent by wallet.
     *
     * @param  owner  Address of owner.
     */
    function removeOwner(
        address owner
    )
        public
        onlyWallet
        ownerExists(owner)
    {coverage_0xc83ff638(0x693ba9fec25e2d3e58c2267136a7900ae7af3f0bfa982544831b3738bdb5d635); /* function */ 

coverage_0xc83ff638(0x33c9aa88e8fcb994b4814fc58dc20675627b53626c3a5e4925db20e2a61a33df); /* line */ 
        coverage_0xc83ff638(0xe478064e75a6eab69befea4b2885bd2e10a90f507972d520e598f7e3dd6040b3); /* statement */ 
isOwner[owner] = false;
coverage_0xc83ff638(0x23cd78a3eef059d7be2294561125979a227a2dbf6311f0a053672d0ef3de2527); /* line */ 
        coverage_0xc83ff638(0x2fb4d1b7d050ef068a9d728ab03a22c94bd1a4e38ba7d3bc4184a814e7b90000); /* statement */ 
for (uint256 i = 0; i < owners.length - 1; i++) {
coverage_0xc83ff638(0x2b7b30717f5c7f82f359e475463540107bea6eb7ed1189ef5da083cbba9fac59); /* line */ 
            coverage_0xc83ff638(0x0de878526cb8945747ea951d4e0758f31111d72438c70d79db58d53e69c28cc7); /* statement */ 
if (owners[i] == owner) {coverage_0xc83ff638(0xc1274365dd5911af804821d4a858d2883219ab8f5eeb082a105ee0964b726be2); /* branch */ 

coverage_0xc83ff638(0x34b78f73956f2ea7c3ca244fe0cf1d71ae5aaacc6ae843832153a2da3de9c276); /* line */ 
                coverage_0xc83ff638(0x28c1d6f22212b3f0bbea9d8cdddceaec96e2a96ee1b867ddad38b86560680668); /* statement */ 
owners[i] = owners[owners.length - 1];
coverage_0xc83ff638(0xd0a19491748cff8ff7466eeb20d9ece9868fec781c4b303e4a08e0cda50419ef); /* line */ 
                break;
            }else { coverage_0xc83ff638(0xa64c8ee8bdd7de9525d1eefaedb060ecdc21bf4942f21b0d382604564305c118); /* branch */ 
}
        }
coverage_0xc83ff638(0x8a57758845a2db29f1f31df6dbece321929d97e760f98448432cbc5bd48665be); /* line */ 
        coverage_0xc83ff638(0x5f6b68d44f07413c8633b1af3a0eae819810e464ad9dbce14a76387390152e0c); /* statement */ 
owners.length -= 1;
coverage_0xc83ff638(0x4d5ab4087a11fe0660ae3a161da94be1fac300b880ee49fb614aa264f2a141b1); /* line */ 
        coverage_0xc83ff638(0xaf59da9bb36146e9f31164d2de8f5e7d4ab76622dee962670f82beeacc4d55eb); /* statement */ 
if (required > owners.length) {coverage_0xc83ff638(0xae4cb146bb798f0525aff801f9913780c9be2c947ad533cad86428dbd0ca0d09); /* branch */ 

coverage_0xc83ff638(0x274c01eb2e162292dce438096815f0bd4b042b816f1bf9f48d09bd10fbb46156); /* line */ 
            coverage_0xc83ff638(0xd5a8de3b44edbf6786e6fa118b85c70ca95f8b1cf2a364a464456700d1fd1cf6); /* statement */ 
changeRequirement(owners.length);
        }else { coverage_0xc83ff638(0x6758f8ae9090d527bfc2f4bd8266a6474a8c4ab632b7932e57ebacd6eb233d87); /* branch */ 
}
coverage_0xc83ff638(0xd14e4cabea79fca887eef1597cdaaf486e922762984c354679dd361bb29b2f44); /* line */ 
        coverage_0xc83ff638(0x3e99de3b923d1f7ef209cf3c666763c77b5ca6a98b717130f296202f8c339ff5); /* statement */ 
emit OwnerRemoval(owner);
    }

    /**
     * Allows to replace an owner with a new owner. Transaction has to be sent by wallet.
     *
     * @param  owner     Address of owner to be replaced.
     * @param  newOwner  Address of new owner.
     */
    function replaceOwner(
        address owner,
        address newOwner
    )
        public
        onlyWallet
        ownerExists(owner)
        ownerDoesNotExist(newOwner)
        notNull(newOwner)
    {coverage_0xc83ff638(0x30ba822fa5f3279a0b1770a314ac5d49395684478cb64c8b5f93279f17c294bc); /* function */ 

coverage_0xc83ff638(0x04e888b34d97f150d6e0211c2e18280e9c7fed3ba99210ecb7523c0f835e5ddc); /* line */ 
        coverage_0xc83ff638(0xdb1f60cec2c7d08ca405067aa6bd0c5a8f2744f2096baf1fbc6a3ff4f715eb8e); /* statement */ 
for (uint256 i = 0; i < owners.length; i++) {
coverage_0xc83ff638(0x0bd532f51a9af763939ec86f13250db426a64057dd0446217e1119d5b74e1a84); /* line */ 
            coverage_0xc83ff638(0x858d575aeb8138d7691a90a8a66508d13276571f72b91be51d6848d2fa834503); /* statement */ 
if (owners[i] == owner) {coverage_0xc83ff638(0x5d425a1e07872be02fd2d8befe1fec87f20c60407eca32806ccd2ca75d81ed6e); /* branch */ 

coverage_0xc83ff638(0x278790244479ecb6cae6ac281cdb5f9696e26ee54a01bc9bea1ea9e37297b3cb); /* line */ 
                coverage_0xc83ff638(0x44ac07f7ecec925a927a62bf0eb371279707981d8434f7e9b727139d462afdcb); /* statement */ 
owners[i] = newOwner;
coverage_0xc83ff638(0xcb15cbf73a1573302e5713558f420c0033dbc383e0f04424c71b0c8f56c0a6c7); /* line */ 
                break;
            }else { coverage_0xc83ff638(0xa4df1c4d5866ef61c579a547759189abafd84a78fea6ad0d480ea66a8380d9f9); /* branch */ 
}
        }
coverage_0xc83ff638(0x45638394a3ee59022221b21a5ad33679be8920706f5f6fab21d5737448083bd7); /* line */ 
        coverage_0xc83ff638(0xdd3cc7163e2761ff9e2b0595207e87c3b8504600ff89b4b746225e85ef098d23); /* statement */ 
isOwner[owner] = false;
coverage_0xc83ff638(0xeb04441ac7630bedda6da2970e74a1c3ba005f9980628593024319b832ff9617); /* line */ 
        coverage_0xc83ff638(0x7d90d69ea2d7ed65e7aae9cab3fccd5b77f27a518c26e4c2f058b60e18f226de); /* statement */ 
isOwner[newOwner] = true;
coverage_0xc83ff638(0xbbfe84a028b4cd0ec2a726a9b32e2e0588b2b888b1b88cfe888df502a25bb031); /* line */ 
        coverage_0xc83ff638(0x9660434ee3e825a2db17a0de11157c99c95f922ac947a569291c7abe8432ff28); /* statement */ 
emit OwnerRemoval(owner);
coverage_0xc83ff638(0x28f3773abf8c789f652d37b4ead4471cd2413b95b2b8a715cafbeb1d056da6c6); /* line */ 
        coverage_0xc83ff638(0xd4288179de3a7173c98444b3ead55006fac270317c4dfb0995a8e39bfdb7c63b); /* statement */ 
emit OwnerAddition(newOwner);
    }

    /**
     * Allows to change the number of required confirmations. Transaction has to be sent by wallet.
     *
     * @param  _required  Number of required confirmations.
     */
    function changeRequirement(
        uint256 _required
    )
        public
        onlyWallet
        validRequirement(owners.length, _required)
    {coverage_0xc83ff638(0x9c81a09afdee0f1879a0a18d8e7a0f53868e40272b15b2a2a2e7a497d1c58140); /* function */ 

coverage_0xc83ff638(0x4588de2c860e7270204397088a9471cc30cda05f36de38ed74d7f9bacb25651c); /* line */ 
        coverage_0xc83ff638(0x87fb7a47510c9f80577954932936c1a54145e435136a238197f07b2128b1aa43); /* statement */ 
required = _required;
coverage_0xc83ff638(0x2f31d107401e26be5a6ce05aecf59952348ff9762999846f4ccfd3bc1f15bc8d); /* line */ 
        coverage_0xc83ff638(0xada38c2f9116980b26a48b6399e4ec7dc142f37f563ce9b64c81d1335a728255); /* statement */ 
emit RequirementChange(_required);
    }

    // ============ Admin Functions ============

    /**
     * Allows an owner to submit and confirm a transaction.
     *
     * @param  destination  Transaction target address.
     * @param  value        Transaction ether value.
     * @param  data         Transaction data payload.
     * @return              Transaction ID.
     */
    function submitTransaction(
        address destination,
        uint256 value,
        bytes memory data
    )
        public
        returns (uint256)
    {coverage_0xc83ff638(0x3a43833870d1e2c4aaf55e27a257363e93db391347fc342579e2b37994db716f); /* function */ 

coverage_0xc83ff638(0xb3aca1487511dd2108be76b61e8d1338d11ae2d8d158d39193750a6fba0c3e4a); /* line */ 
        coverage_0xc83ff638(0x7e55f3228697fbd589b909a0784dec066134c3af2af3f0cc319741cc55373fd7); /* statement */ 
uint256 transactionId = addTransaction(destination, value, data);
coverage_0xc83ff638(0x9a03e54e767edd13878fbc28254c4aef3bb25012c6c4fc1a329d31307b728dcf); /* line */ 
        coverage_0xc83ff638(0xc64a11fe963cc3e699ca06bb845ec604a71354dfdb4f448e5955a91ccefc7ef0); /* statement */ 
confirmTransaction(transactionId);
coverage_0xc83ff638(0x8cb27c5afe063571676abadbd965b76fef523a80267e2c724be2fcb929cac01f); /* line */ 
        coverage_0xc83ff638(0xff16d571fe5f8e857ef140c1f37b695afb2818e40b757e83fd25f18ead255244); /* statement */ 
return transactionId;
    }

    /**
     * Allows an owner to confirm a transaction.
     *
     * @param  transactionId  Transaction ID.
     */
    function confirmTransaction(
        uint256 transactionId
    )
        public
        ownerExists(msg.sender)
        transactionExists(transactionId)
        notConfirmed(transactionId, msg.sender)
    {coverage_0xc83ff638(0x9079a613e7366dfe52b05231332638f026b775916b15c0cadc084d97c0d86010); /* function */ 

coverage_0xc83ff638(0x6ce7d6278b34f240e5386b33a50cd4fe20bd2b0e31fb03c528c7a31e75daa0fa); /* line */ 
        coverage_0xc83ff638(0xb66402ad3a7be5ff281edbcac5dcdd49601c94c3c990b9eb8b3d7f44b1305ede); /* statement */ 
confirmations[transactionId][msg.sender] = true;
coverage_0xc83ff638(0x07473715e63b4f060e9aa6574f86567519b7daced2039e1d99c3a8d7bb76287f); /* line */ 
        coverage_0xc83ff638(0x4761c5dd769fb1be7a331128b54afdd4f42dac078d95f8ef27692949d1b9711f); /* statement */ 
emit Confirmation(msg.sender, transactionId);
coverage_0xc83ff638(0x1fb21ce77bb453711971c0b37f5363147e7441ff13aab60952627399bbaa0af4); /* line */ 
        coverage_0xc83ff638(0x73904c4168f3ef47b5f7da27be5781dd4bcd2605badcfd45ce9fff25b0f98bf5); /* statement */ 
executeTransaction(transactionId);
    }

    /**
     * Allows an owner to revoke a confirmation for a transaction.
     *
     * @param  transactionId  Transaction ID.
     */
    function revokeConfirmation(
        uint256 transactionId
    )
        public
        ownerExists(msg.sender)
        confirmed(transactionId, msg.sender)
        notExecuted(transactionId)
    {coverage_0xc83ff638(0x432531c545570a2f6f44dddffe6d08efe3a02c0c0e8aecb8defee6ce527059f3); /* function */ 

coverage_0xc83ff638(0x1a6fb0290b7d106adba15b6109b6c9b2ee86357cd2932039b53ffc98e282a1f0); /* line */ 
        coverage_0xc83ff638(0xafc8eff2bbcb340c6f300f58b985f8f971c5f28ec7e0006245fa7551fe866c64); /* statement */ 
confirmations[transactionId][msg.sender] = false;
coverage_0xc83ff638(0x6de51f3566d7a57054fffeb34e3d6b8a03678a78a4ad9ff1463b08f6373fd7d1); /* line */ 
        coverage_0xc83ff638(0xff869918f24f8f5051594118a93ef8a895dbc9f7025cf1601d46469a254e8123); /* statement */ 
emit Revocation(msg.sender, transactionId);
    }

    /**
     * Allows an owner to execute a confirmed transaction.
     *
     * @param  transactionId  Transaction ID.
     */
    function executeTransaction(
        uint256 transactionId
    )
        public
        ownerExists(msg.sender)
        confirmed(transactionId, msg.sender)
        notExecuted(transactionId)
    {coverage_0xc83ff638(0x154ab3d980fe180261599ea439efb11cab033c17c9a2f24d20d6b5c2f37d4658); /* function */ 

coverage_0xc83ff638(0x9f584dbb2d5a5683c18537344db78b7f0927940db61fc446bd25ad19297deed1); /* line */ 
        coverage_0xc83ff638(0x12dcf967cd2ee26cbb9b2c17b3b7eb895318c4db38da174606c3ec0b7c7d7d87); /* statement */ 
if (isConfirmed(transactionId)) {coverage_0xc83ff638(0x6f8e1ab619a69d2dd39d02503c56db24eec2ed087e0dcd846627ca8c38876e64); /* branch */ 

coverage_0xc83ff638(0xf1bddc8b9ceb1210ae8e168ca169978de5e824615902a3ff4350c534050f79db); /* line */ 
            coverage_0xc83ff638(0x68d9c6aee1305c6f536ac37aeb657c9b2d57357a9059341d7badb8b57fc606bb); /* statement */ 
Transaction storage txn = transactions[transactionId];
coverage_0xc83ff638(0x9f0a99d5ecb12d1b52d2388dd1003c4563f3b8b5c7ccf0feb8289f56a04e16bf); /* line */ 
            coverage_0xc83ff638(0x76d823ac9bead0370990f87d50b8b6b2baad79340fbed3b5c45c6d23bcae35f3); /* statement */ 
txn.executed = true;
coverage_0xc83ff638(0x0867c2d6f182c5123654ccb57bc2f9e894dfcff5c0650aba25bd54fee0deded6); /* line */ 
            coverage_0xc83ff638(0x726bbabf9e276eaf5923dc4b911b576835251b45a858c22ca5c83768173c79b4); /* statement */ 
if (externalCall(
                txn.destination,
                txn.value,
                txn.data.length,
                txn.data)
            ) {coverage_0xc83ff638(0xa31c00cb8c1198df492e2895117a46c6301d719812de2c809ff74c8332809b45); /* branch */ 

coverage_0xc83ff638(0xec85dbb7e6a955bea1acd2f1ee0bc3e20b78132e185cb195347fa05fa90fd935); /* line */ 
                coverage_0xc83ff638(0x759730ab85959b22d9aa97d6a3c5842ab2724c882507b30f4239170d4fc8cf9a); /* statement */ 
emit Execution(transactionId);
            } else {coverage_0xc83ff638(0x24d54f6f0baa962ff21b95ae5d0df88c9fbeeb88b8a056bd014577fd0ab21b79); /* branch */ 

coverage_0xc83ff638(0x8024466bb821614a2d927492bf1a62bee0e066d99ec39f899c31622378339b2d); /* line */ 
                coverage_0xc83ff638(0x85d950f5dcd96fed34a20813576528729e1afac4d5f53a7f8aede7f40c6d7339); /* statement */ 
emit ExecutionFailure(transactionId);
coverage_0xc83ff638(0x0e6ac8fb2aa3722197459ac430bbd33982eed7d9a78640b30f0f4dc66e1ed58f); /* line */ 
                coverage_0xc83ff638(0xd791407916d7fe1765644ef50840dc21e1c9aa3e4c250021df35ed09aa114f91); /* statement */ 
txn.executed = false;
            }
        }else { coverage_0xc83ff638(0xd0d296762dac2c8eb9ce213dbb7bd3e43c1fcda77b35854d5a8aa2fdb307a8fd); /* branch */ 
}
    }

    // ============ Getter Functions ============

    /**
     * Returns the confirmation status of a transaction.
     *
     * @param  transactionId  Transaction ID.
     * @return                Confirmation status.
     */
    function isConfirmed(
        uint256 transactionId
    )
        public
        view
        returns (bool)
    {coverage_0xc83ff638(0x5f46582e316d3b96ec069c7cd70a8f661525a9aa47ab1e5085077630ecbccdb1); /* function */ 

coverage_0xc83ff638(0x37789c79e9be8ba907db222f122051b3db7165114a21e81370a756a91579cd9e); /* line */ 
        coverage_0xc83ff638(0xa714d5421c3883c92185e86c9f085dc85a942cbc66af4c528d68766ab4d09b75); /* statement */ 
uint256 count = 0;
coverage_0xc83ff638(0x466b991b2b93fb127be0b7cc05cf53b639f48e5e55867741e5df5c752adeaacf); /* line */ 
        coverage_0xc83ff638(0x61f4fffd2e2eab85028fc9c386df6e6e4715f8efc56b4def33e8f70dc642f471); /* statement */ 
for (uint256 i = 0; i < owners.length; i++) {
coverage_0xc83ff638(0x7cca1d32af0abde0a0b6c3334f387aad2254eb8aaa4a373809454c3bdf17b68f); /* line */ 
            coverage_0xc83ff638(0x10b9ffc1b356de359cad1585594bf23b90fc87a2378caf727ed0e41a667451ad); /* statement */ 
if (confirmations[transactionId][owners[i]]) {coverage_0xc83ff638(0x18cb3a828eb241c175d245e28d5db0a2cccbb6ea8e609f80f2435db8fe9ab425); /* branch */ 

coverage_0xc83ff638(0x18d625677a66a7bd7c5691779cd14aee10b3ceb3aa3f78b77dae3d2bb59f6a8b); /* line */ 
                coverage_0xc83ff638(0x1343432e4d2c855f3ccdb34d9e94c470e4bb73e3ba4adb9913a1e8d2d5a3823f); /* statement */ 
count += 1;
            }else { coverage_0xc83ff638(0x1b80768a92071f06ed291a65a5553c86c58c8ac79fedebab6553b846675fc6c4); /* branch */ 
}
coverage_0xc83ff638(0x7eb2703577c0d3ae01de7f7a82f94cb4d4a102378cfb1d1b21d7bccf2ec8bab2); /* line */ 
            coverage_0xc83ff638(0x70a112a9a4be651541a42b505cc80be4c10b36234f6257b9eaf88ed1e7dd3d2e); /* statement */ 
if (count == required) {coverage_0xc83ff638(0x8e49aee875c9e4586a339a0e4d4a1c8bd6c1846a27262b04ab81dd9a9d722c93); /* branch */ 

coverage_0xc83ff638(0x836b4c524d2f5b5b7c963eda5ab324a8a950d09c63ea42393338cfbcc6c19d3a); /* line */ 
                coverage_0xc83ff638(0x07585983b5e28dc8f77f69a98859c913c6a6dbda4b2a6e99bd75719288665af9); /* statement */ 
return true;
            }else { coverage_0xc83ff638(0xcbb6bb232b6f74e3837573e8540fd4ebd74070e0f32f1c5e5978d5a93859ecce); /* branch */ 
}
        }
    }

    /**
     * Returns number of confirmations of a transaction.
     *
     * @param  transactionId  Transaction ID.
     * @return                Number of confirmations.
     */
    function getConfirmationCount(
        uint256 transactionId
    )
        public
        view
        returns (uint256)
    {coverage_0xc83ff638(0xe3b4817b8ebd354e1cd8fd91b8a8199f3a34d1f1bb44130bf422c46674486388); /* function */ 

coverage_0xc83ff638(0x504750cfcd27b12140d8f207fc72231de0ab8cac12496dcfac5a5de23ca23c11); /* line */ 
        coverage_0xc83ff638(0x872ecc140999c9747d6093ed5a41300696d1bcf67e14fbf5c8540d272916f3e6); /* statement */ 
uint256 count = 0;
coverage_0xc83ff638(0x45e966d83d57f889208839450ec61e8893615842290fde2bfe2a7a407abcd24d); /* line */ 
        coverage_0xc83ff638(0xcfd3fc346286ca9966ec11bd859060f1b318ee5f6dc8ee237aade9f77aa7c05e); /* statement */ 
for (uint256 i = 0; i < owners.length; i++) {
coverage_0xc83ff638(0xe67a9e87aff9e2cfaf46f896c74ab8085993dcd10df8a1605cad7015961f0016); /* line */ 
            coverage_0xc83ff638(0x71ff569ae7aa5722c16cf8f344509ae83ec59a11df8acb6fb9240088570f581d); /* statement */ 
if (confirmations[transactionId][owners[i]]) {coverage_0xc83ff638(0x203639d310ceae1fb1f0432d82b5902069aceacde0a53fefa7f3e422365d1a5f); /* branch */ 

coverage_0xc83ff638(0x49e7386d2038f0b1fa28dd1d39ee4c0756e433f45f0622fefb4063ced8699e7e); /* line */ 
                coverage_0xc83ff638(0x9013689d822ae290f993499421cb01bf0b69933aa979774cb321b94d0828c385); /* statement */ 
count += 1;
            }else { coverage_0xc83ff638(0xf410ddea6072362c51af6a517bd6193628df01eaed07709f8856d5fdc31a3ae7); /* branch */ 
}
        }
coverage_0xc83ff638(0xc14bb92a4ef46b295d744b578fef54b8bb6ce83282929de3767f00aadd717185); /* line */ 
        coverage_0xc83ff638(0xefb054fe31c52b17a41b9f21a6f796bc3009351578de2e747f926f9baa6c9305); /* statement */ 
return count;
    }

    /**
     * Returns total number of transactions after filers are applied.
     *
     * @param  pending   Include pending transactions.
     * @param  executed  Include executed transactions.
     * @return           Total number of transactions after filters are applied.
     */
    function getTransactionCount(
        bool pending,
        bool executed
    )
        public
        view
        returns (uint256)
    {coverage_0xc83ff638(0xa2bbccac35cc067b8f26387f792d341743ea763a5cd0ea00f0a35f9fa6ff42ec); /* function */ 

coverage_0xc83ff638(0x968f020598461109361f37d3a42409a1b93192535ee969111a38df8b60fd157f); /* line */ 
        coverage_0xc83ff638(0xdf07ab61ef56b016223c0732137f1249ee103b4008c004d2b576fb28fc4735b5); /* statement */ 
uint256 count = 0;
coverage_0xc83ff638(0x30ff147d9423089d0cd07b7ac049b6fef1d560fe7218a393c621b0ab684db9c9); /* line */ 
        coverage_0xc83ff638(0xfab25c30fd69b44236b60923ad960a17d6132612eb61af60b47e263603520f5f); /* statement */ 
for (uint256 i = 0; i < transactionCount; i++) {
coverage_0xc83ff638(0xa3343a86ceb4d19704efa63c22f4fdebde7d122bac815777aa00422d7cd94e38); /* line */ 
            coverage_0xc83ff638(0xe17c1351f3127b07919f7002c4948aad22dbb53a91b9c35c3fd59771bb344b44); /* statement */ 
if (
                pending && !transactions[i].executed
                || executed && transactions[i].executed
            ) {coverage_0xc83ff638(0x488b1d46b3ff9d89b9792cdf73fabd8f1751e236f9beec4ed1386c461f2f4b3d); /* branch */ 

coverage_0xc83ff638(0x73f42f2aad761b739e586c70b08a130c6257569e3aa352ba567d49f4c80d2376); /* line */ 
                coverage_0xc83ff638(0xe3fddfd7b572a84cbd08e224a1ea74e8b089e9737e1726efda878ffe4dfa98f2); /* statement */ 
count += 1;
            }else { coverage_0xc83ff638(0xb55a090725dba03612e27373b077968babc00b125eccc02d59aaff0a93f3956a); /* branch */ 
}
        }
coverage_0xc83ff638(0x38cdde5073642f066e7dc27d9d8c330f7ae08484f879be656f443f868d037682); /* line */ 
        coverage_0xc83ff638(0x33bdd3de5cde3594b5498d1b70513a3c7a593e3701c7603528808f844bcba2b9); /* statement */ 
return count;
    }

    /**
     * Returns array of owners.
     *
     * @return  Array of owner addresses.
     */
    function getOwners()
        public
        view
        returns (address[] memory)
    {coverage_0xc83ff638(0xc40abe9baf3e0cd24af6d9974b2f84ab3e08c1abc832683e5abf2e0a0b9af146); /* function */ 

coverage_0xc83ff638(0x52ffbbcc1ecc2c93c98031b42649c8e5d955e9eb38069ed2889d3263d64aba7a); /* line */ 
        coverage_0xc83ff638(0x642e6f65199335b0c443e660204a5e9cd9e3beeedfd09cc0244bffc025449c64); /* statement */ 
return owners;
    }

    /**
     * Returns array with owner addresses, which confirmed transaction.
     *
     * @param  transactionId  Transaction ID.
     * @return                Array of owner addresses.
     */
    function getConfirmations(
        uint256 transactionId
    )
        public
        view
        returns (address[] memory)
    {coverage_0xc83ff638(0x4a570d2eb527ff45e0ab3bc3a9f25bad37db607f1e6aaca1476279d4187ebe2d); /* function */ 

coverage_0xc83ff638(0xd267d64919e4f4826e85cd09c6c743a512086d7e82baf18ff16993ab92beff25); /* line */ 
        coverage_0xc83ff638(0x9d7766695f0b6a84c51d094d5d06066ddeadc1b7e34e4f12ecd075147fc63768); /* statement */ 
address[] memory confirmationsTemp = new address[](owners.length);
coverage_0xc83ff638(0x245f2e62fe0d6bb53212b9018d5b03ab9a1d8c72e38ca55b740012631d8e5785); /* line */ 
        coverage_0xc83ff638(0x3d6ee8667e647f8b046d1d90b2f309d466a290d87581a1ec1a7e520d0a14ec2f); /* statement */ 
uint256 count = 0;
coverage_0xc83ff638(0x14592d34bd8023233d9f4242dcda0e0f60115b5754859ba5dd766a3a305514d7); /* line */ 
        coverage_0xc83ff638(0x7b2ccac218687ff3ae84ed45291bc4c18ccfe7456e2eb6d234c777e887576c13); /* statement */ 
uint256 i;
coverage_0xc83ff638(0x4233890a89c1f518d295bb32d460e0ae407826ea2c93b721815720fca563b880); /* line */ 
        coverage_0xc83ff638(0xf4f3825c347cbed4e3b828c17ca05eb45757723da9dfbdf549910361249ca7a3); /* statement */ 
for (i = 0; i < owners.length; i++) {
coverage_0xc83ff638(0x2b51595b473e30cf91fd2ce2bb285b407f40d4fbe24bdea36ba03f9476e9be19); /* line */ 
            coverage_0xc83ff638(0x885c8dcff159abab35eeb5bb4441eb2cee7389d35d6e7443ee20704484b5e9e9); /* statement */ 
if (confirmations[transactionId][owners[i]]) {coverage_0xc83ff638(0x37687fdbb7694a99a2b9c5bf95c315c9753a6d49aebcd933b7df01bd9104b1ad); /* branch */ 

coverage_0xc83ff638(0xf89cbdbf150dc22acaae97ef317c8ab15b70336df043a995a2b000f6c2f824e3); /* line */ 
                coverage_0xc83ff638(0xe01d5acda7e8212cdff9d5799bbf346a6532b5049b86cbf4a233e208240f6ba3); /* statement */ 
confirmationsTemp[count] = owners[i];
coverage_0xc83ff638(0xb1ed6a3611dd3a66af2b8bd4218623a76072fb456d31d339a3ebefab9135eebb); /* line */ 
                coverage_0xc83ff638(0x2a4d29a70398b05af01787b396350bbed3f252616e85ccfbf541d1c0f42ddcad); /* statement */ 
count += 1;
            }else { coverage_0xc83ff638(0x51c570045be9571c1bd746d26ce5f567b0100197ef2aa7524be2535be054f606); /* branch */ 
}
        }
coverage_0xc83ff638(0xd71a15975070a63de673aff4a789c902dc58f75746ce7001efb0b11c4fc456cd); /* line */ 
        coverage_0xc83ff638(0x4927fa3d35b20aff5f8077cf51116a1576163f23bb116373eccbc9f211a4e3e4); /* statement */ 
address[] memory _confirmations = new address[](count);
coverage_0xc83ff638(0xb7afab1def070a7a25ddbf41818b68daa3c7f7c28aa96a950014cb66dd7a974d); /* line */ 
        coverage_0xc83ff638(0x65a085f1e0d6652b0abb0508e1fa7bcee59922d21b1daad9657f93152176caa2); /* statement */ 
for (i = 0; i < count; i++) {
coverage_0xc83ff638(0x2d6b5ce46fb38ee754ab4dfcdd17c1e8fd12b472b2350202294206cbcaaf3692); /* line */ 
            coverage_0xc83ff638(0x804d5cf97d97fc092f784c794a35d8d12289bda42486e3d3484985d0cf623500); /* statement */ 
_confirmations[i] = confirmationsTemp[i];
        }
coverage_0xc83ff638(0xe05b277747115cfb156616cbae552dca0c538afc27b5acae25e616a8b796bb99); /* line */ 
        coverage_0xc83ff638(0x162ca82bc9da0024fe26935e62d48d7a32668ffb5a6f619a46f5a42683bbf546); /* statement */ 
return _confirmations;
    }

    /**
     * Returns list of transaction IDs in defined range.
     *
     * @param  from      Index start position of transaction array.
     * @param  to        Index end position of transaction array.
     * @param  pending   Include pending transactions.
     * @param  executed  Include executed transactions.
     * @return           Array of transaction IDs.
     */
    function getTransactionIds(
        uint256 from,
        uint256 to,
        bool pending,
        bool executed
    )
        public
        view
        returns (uint256[] memory)
    {coverage_0xc83ff638(0x455690a9e21c51bd74bf080de8616b4ab080376262abe7fb46e7b4f826b2016d); /* function */ 

coverage_0xc83ff638(0x40831781e499a38f8b5602c8c3d740f52d41b9586ac1144cbe69f5f4e2920085); /* line */ 
        coverage_0xc83ff638(0x110cbf9895801f24598005626aed30369fc136b7e3498ceaea1e5d158619a1c9); /* statement */ 
uint256[] memory transactionIdsTemp = new uint256[](transactionCount);
coverage_0xc83ff638(0x1815adeef495a296de332ca56e3f07cf214c8a38418b74ec40ba9f41f01678d0); /* line */ 
        coverage_0xc83ff638(0xdd6ece000dab1a499376d8f32a0bb9c28f9d360325561a2d9e788d4571d2de53); /* statement */ 
uint256 count = 0;
coverage_0xc83ff638(0x981576d01af5e0f9ae5116192700120f54fd03cd490c3fdc8f636510382969aa); /* line */ 
        coverage_0xc83ff638(0x203d3c0f6f3f244c71d1824154571c83b8d8595fc7d36499abc4a00d2a9b7daf); /* statement */ 
uint256 i;
coverage_0xc83ff638(0x9619967759295056957b522c7fa201fcb73126bdcb15a857c428e3927a66585e); /* line */ 
        coverage_0xc83ff638(0xee0e8570fc8de10a86613ba919a509d1e6d701bc4cd1df6bee3d310ae3e7f3d5); /* statement */ 
for (i = 0; i < transactionCount; i++) {
coverage_0xc83ff638(0x298465063311c985f8e6ebb363fe4c982eaac1359d5a139e46c6bc95f8578935); /* line */ 
            coverage_0xc83ff638(0xcac9605c55c69f9b8ba255bae5fae35924250816b7cdafe50518c9a92f6327ef); /* statement */ 
if (
                pending && !transactions[i].executed
                || executed && transactions[i].executed
            ) {coverage_0xc83ff638(0x1d3d26415861e6419e81cdde0b01a1585195234784490a3a4636dfc146d3e586); /* branch */ 

coverage_0xc83ff638(0x9b2bc070ffc080c127be5e6d8b94da4a434c27e2de05f773895ea843853e7bb6); /* line */ 
                coverage_0xc83ff638(0x9920f0d4528fa71dbbf8b16127267adb52bc8072299e0bc3816c8ea2daff8801); /* statement */ 
transactionIdsTemp[count] = i;
coverage_0xc83ff638(0xd7dc4e6fb03cc9de4f27e2e19134657e7050d024435efa404f246f39b1ee95df); /* line */ 
                coverage_0xc83ff638(0x24cd582d5d4597b682cd162ab483e159112f9d1d54c63a950cd3051aa855f6ed); /* statement */ 
count += 1;
            }else { coverage_0xc83ff638(0xb3131d98649d21b782f93bfee5de154e5d614f8b8df0ec9ef72d3dbd3327b65f); /* branch */ 
}
        }
coverage_0xc83ff638(0x620c2963346a56b3fcfdd5d9a46f197db8dd1ba2da1d5dde0b4a3cf1e6795134); /* line */ 
        coverage_0xc83ff638(0xca042bfa3de10f9990fbb5c8ebea941b570585976929ba0dcf3efddad1ba798d); /* statement */ 
uint256[] memory _transactionIds = new uint256[](to - from);
coverage_0xc83ff638(0x65a66fba5f39c08eabd7a7e5421afc3b52f60f1a5d1dd6d1467683794d4635d5); /* line */ 
        coverage_0xc83ff638(0x471f09add489dfdbd601d64e10314ab52cfb2bdfb580fd137d0f321f8a2c7236); /* statement */ 
for (i = from; i < to; i++) {
coverage_0xc83ff638(0xfa2d8ea614e955adf106bd0e63207a24c70c8ade304bc0ddbee3e86e21620f25); /* line */ 
            coverage_0xc83ff638(0x34546982ad08ed0b9080fa4dbed32025241dccebbc407adfe24c52f2c5251080); /* statement */ 
_transactionIds[i - from] = transactionIdsTemp[i];
        }
coverage_0xc83ff638(0x0aa8cb2262fd27a4ddb5bf6611dcab9046ec550fac70250dfb59e8927599bd18); /* line */ 
        coverage_0xc83ff638(0xc1df0a32cd7315357e1f0cd1d0f425c145795b91eb503bb52763a89da1f90093); /* statement */ 
return _transactionIds;
    }

    // ============ Helper Functions ============

    // call has been separated into its own function in order to take advantage
    // of the Solidity's code generator to produce a loop that copies tx.data into memory.
    function externalCall(
        address destination,
        uint256 value,
        uint256 dataLength,
        bytes memory data
    )
        internal
        returns (bool)
    {coverage_0xc83ff638(0x7aec6d31f0da2a89552841e36fb5c66c9e69849816a16fea1ce24364d84540f3); /* function */ 

coverage_0xc83ff638(0x5eed2682d8d67f682b8c4ec475c09701ae2b95edb4896c9fab8ca17770943c99); /* line */ 
        coverage_0xc83ff638(0x3df110117eb59ef5b378467b7d940731d3ca43a61d86b963f99e0fbc0d6be900); /* statement */ 
bool result;
        /* solium-disable-next-line security/no-inline-assembly */
coverage_0xc83ff638(0xa5f8fdbe144f5f442841ef31b3df328139abf510b30939bd554b0c6e509dc8a0); /* line */ 
        assembly {
            let x := mload(0x40)   // "Allocate" memory for output (0x40 is where "free memory" pointer is stored by convention)
            let d := add(data, 32) // First 32 bytes are the padded length of data, so exclude that
            result := call(
                sub(gas, 34710),   // 34710 is the value that solidity is currently emitting
                                   // It includes callGas (700) + callVeryLow (3, to pay for SUB) + callValueTransferGas (9000) +
                                   // callNewAccountGas (25000, in case the destination address does not exist and needs creating)
                destination,
                value,
                d,
                dataLength,        // Size of the input (in bytes) - this is what fixes the padding problem
                x,
                0                  // Output is ignored, therefore the output size is zero
            )
        }
coverage_0xc83ff638(0x9b593638421f70de7a87c90a7244953e514f3942feb4b17817b5ef602ed5482a); /* line */ 
        coverage_0xc83ff638(0x8d3dfc8dc5d5e5a73a2a422d1ae9c3678d4f389ddfcdcaa93e012ab429499d0c); /* statement */ 
return result;
    }

    /**
     * Adds a new transaction to the transaction mapping, if transaction does not exist yet.
     *
     * @param  destination  Transaction target address.
     * @param  value        Transaction ether value.
     * @param  data         Transaction data payload.
     * @return              Transaction ID.
     */
    function addTransaction(
        address destination,
        uint256 value,
        bytes memory data
    )
        internal
        notNull(destination)
        returns (uint256)
    {coverage_0xc83ff638(0x1af3bfad474fb6d6b4c36a3c364cbbb2231b1adee04be41b8fba8ca4fea0644f); /* function */ 

coverage_0xc83ff638(0x5c63b48a6b410dde2000cbd63267e9fff548190304f101eb65953a3a6219afc9); /* line */ 
        coverage_0xc83ff638(0x1dbc671958b07efb7f951dee4ba7bc5267beab1af086b252a5223a0e6ae8ffb5); /* statement */ 
uint256 transactionId = transactionCount;
coverage_0xc83ff638(0xad5ac514a59531c62da317418bd2815ae67602b2d05479fe3d855f301e2e102b); /* line */ 
        coverage_0xc83ff638(0x182fa0cf49821a02b7328a1f2532454e0268807c3517d203a15672a92fd9e74d); /* statement */ 
transactions[transactionId] = Transaction({
            destination: destination,
            value: value,
            data: data,
            executed: false
        });
coverage_0xc83ff638(0xed79a4c5e0593d8be1545fa15070ef84f6a64c8c480439b06bd0a14846096783); /* line */ 
        coverage_0xc83ff638(0x8aeb75bd4ccd61847d3a7060b1b2fb5889dfb521fb44db78bac193f3a95bd270); /* statement */ 
transactionCount += 1;
coverage_0xc83ff638(0x899b3c7bd11fb777c9c5af21cb0b040310d7f4a4b54f313fed80316f2f7f975f); /* line */ 
        coverage_0xc83ff638(0x93402d1e763185add4cdcf38131fb57a45a92c3fb648eaba793708b28e048e7d); /* statement */ 
emit Submission(transactionId);
coverage_0xc83ff638(0x137716b723612ccba903876bf59f633e1f2828540fa365dcc7185489c2f39377); /* line */ 
        coverage_0xc83ff638(0x15bc6695667e892d781d447579fa5c4e851d1b6b595b55f0febd996634adb48a); /* statement */ 
return transactionId;
    }
}
