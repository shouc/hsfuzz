import os

dir = "/tmp/cbc"

projects = [
"177b91c9e380d01dfd3a4df11c05ba06",
"f3223696-be31-4e8d-9665-e9cadc7631f3",
"1598e2843c6edac99bd020c6124558a4",
"c08610e6a0328d30007bad377a0ecd7d",
"8589cd1cbbeb0f683ef807ffcb7d37c4",
"f27e400a235144a1ab923c7cb8a7ec92",
"1aaeea7d3302e944347a77b5b4ab82d3",
"11ac7f152a8b05a8e2edc4b4209f5c59",
"10f4eea25ec8e93c49e38d577afc457b",
"e8c3347076e883e0621128ab4bceb20b",
"825de4b43e68dae4149d90e3dc1e564c",
"a9b8fdced342e49badfc5e8c15cf7c84",
"09310ea0245e78750e6595fb90c28a5b",
"932cc0169a8d5a8ce56eb8b3ea6edb6a",
"5fbe640ab351a3b1288eca2c246aeb2d",
"97b75f8be7e46c575bb7d14626529d64",
"2f784c5706c7f30982c1b38b729e90e2",
"129d243378e1e9f956d21947e3a9731a",
"cac954cb5332867b74f22f49e10a36c4",
"2e13840cd9f8687032d0633cd6df38fa",
"4de25bc377ef15d7425efcf6c22ca004",
"5ddc547e489757731d70ea6d552191b8",
"24c501dd-fe97-421f-98cd-392cf79ef999",
"d0b4a9fd6cb32b0ace80f6463f1bb609",
"051278ebbfaef2a74f23252b5ec97dd4",
"1be5aa53b82b57138b87226bb669da7b",
"b412893b21b3e067e28b5ce6937828c8",
"48f8f3a24fa226f0d6bd9a6af69651d4",
"d14f1e34ac09098be6f1dd49313e8111",
"80ae3c41a12049c6fac1bd174cffd470",
"e551b6ba8abfc31de1214b5dc2d70d79",
"e7fd0b2585a614a5ddbc929ee2f14ba4",
"9c18d5639948a80538e982bba5831f13",
"4311c1c632be687d65240bdb64b97397",
"d948bddb463c2a31e3060fa1b474143c",
"3c756a9277ebb8f83b06dcbaac0d15e6",
"f6f7d6b7-bdbc-47ec-84cb-c251f926e0f9",
"1d404eac6640ffd0eba13257f2aa7332",
"5d89731d56d5459ac26cc3c09b46277a"
]

def comp_should_eq(a, b):
    a = int(a)
    b = int(b)
    if a > 6 or b > 6:
        return True
    if a in [4,5] and b in [4,5]:
        return True
    if a in [2,3] and b in [2,3]:
        return True
    return a == b

ft = [x[:-1] for x in open("funct.txt").readlines()]
ft = {x.split(" @ ")[0]: eval(x.split(" @ ")[1]) for x in ft}


def internal_func(funct_name, args):
    if funct_name in ft:
        func_arg = ft[funct_name]
        if len(args) > len(func_arg):
            return False
        for k, i in enumerate(args):
            if i not in func_arg[k]:
                return False
    return True

for folder in projects:
    cter1= 0
    gcter1 = 0
    cbcs = eval(open(f"/home/shou/repos_ok/{folder}/cbc.txt").readline())
    for i in cbcs:
        cdir = dir + "/" + i
        with open(cdir) as fp:
            lines = fp.readlines()
            for j in lines:
                args = [x for x in j.split(":")[-1]][:-1]
                func = ":".join(j.split(":")[:-1])
                if func != "comp":
                    if internal_func(func, args):
                        # print(1)
                        cter1 += 1
                    gcter1 += 1


    cter = 0
    gcter = 0
    cbcs = eval(open(f"/home/shou/repos_ok/{folder}/cbc2.txt").readline())
    for i in cbcs:
        cdir = dir + "/" + i
        with open(cdir) as fp:
            lines = fp.readlines()
            for j in lines:
                args = [x for x in j.split(":")[-1]][:-1]
                func = ":".join(j.split(":")[:-1])
                if func != "comp":
                    if internal_func(func, args):
                        # print(1)
                        cter += 1
                    gcter += 1
    try:
        cbcs = eval(open(f"/home/shou/repos_ok/{folder}/cbc3.txt").readline())
        for i in cbcs:
            cdir = dir + "/" + i
            with open(cdir) as fp:
                lines = fp.readlines()
                for j in lines:
                    args = [x for x in j.split(":")[-1]][:-1]
                    func = ":".join(j.split(":")[:-1])
                    if func != "comp":
                        if internal_func(func, args):
                            # print(1)
                            cter += 1
                        gcter += 1
    except:
        pass

    print(folder, cter1, gcter1, cter, gcter)
