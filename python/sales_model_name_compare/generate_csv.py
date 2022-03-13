# 전체가 판매모델명을 포함하는지
# suffix가 없는 데이터는 없는지

import json
import os
import csv
from collections import defaultdict

os.chdir("./python/sales_model_name_compare")

gmes_new = "./gmes.csv"
productmodel_file_new = "./productmodel.csv"

gmes_files = [
    "./GMES - 01647043203849-61e1eadc:/54eywp23sq7hfoo3gnm5l3hk64.json",
    "./GMES - 01647043203849-61e1eadc:/klzt7wy3iq3dxa5zhosjahq2mq.json",
    "./GMES - 01647043203849-61e1eadc:/kp6jb4u6jm4elmuy5t4f4vc644.json",
    "./GMES - 01647043203849-61e1eadc:/q7tgsk5hiy2hxglaogwc5yypve.json",
    "./GMES - 01647043203849-61e1eadc:/r2ppbzxvlm3dbges7nglp37khm.json",
    "./GMES - 01647043203849-61e1eadc:/u43qxkthhu6pfhlxdp55enhyti.json",
    "./GMES - 01647043203849-61e1eadc:/vwixjroqhazwnosx6kg66vyjku.json",
]
# prodmodel_file = "./productmodel/kic-01647073802328-e4ace499.json"
# prodmodel_file = "./productmodel/aic-01647073802106-37b7ad34.json"
prodmodel_file = "./productmodel/eic-01647073802650-64eeba81.json"

gmes_columns = [
    "deviceId",
    "macAddress",
    "serialNo",
    "setSerialNo",
    "boxId",
    "salesModel",
]


def percent(target):
    return "{:,.2f}".format(target * 100)


def _cs(target):
    return "{:,}".format(target)


def make_refined_productmodel():
    def to_mac(device_id):
        suffix = device_id[24:]

        ret = ""
        for i, c in enumerate(suffix):
            ret += c
            if i % 2 == 1:
                ret += ":"

        return str.upper(ret[:-1])

    items = []

    with open(prodmodel_file, "r") as f:
        for i in f.readlines():
            item = json.loads(i)

            new_item = {}
            for k in item["Item"].keys():
                v = item["Item"][k]
                new_item[k] = v[list(v.keys())[0]]

            new_item["macAddress"] = to_mac(new_item["deviceId"])

            items.append(new_item)

    columns = ["deviceId", "serialNumber", "macAddress", "salesModelName", "writeBy"]

    with open(productmodel_file_new, mode="w", encoding="utf-8") as f:
        writer = csv.DictWriter(
            f, delimiter="\t", fieldnames=columns, extrasaction="ignore"
        )

        writer.writeheader()
        writer.writerows(items)


def make_refined_gmes():
    def make(file, items: set):
        with open(file, "r") as f:
            for i in f.readlines():
                item = json.loads(i)

                new_item = {}
                for k in item["Item"].keys():
                    if k not in gmes_columns:
                        continue

                    v = item["Item"][k]
                    new_item[k] = v[list(v.keys())[0]]

                if len(new_item.keys()) <= 0:
                    continue

                items.add(json.dumps(new_item))

        print(f"items : {len(items)}")

    items = set()

    [make(f, items) for f in gmes_files]

    items = [json.loads(i) for i in items]

    with open(gmes_new, "w") as f:
        writer = csv.DictWriter(f, fieldnames=gmes_columns)

        writer.writeheader()
        writer.writerows(items)


def seperate_by_device_id():
    device_ids = {}
    non_device_ids = []
    dups = defaultdict(list)

    with open(gmes_new, "r") as f:
        reader = csv.DictReader(f)

        count = 0
        for idx, row in enumerate(reader):
            device_id = row.get("deviceId")
            if not device_id:
                non_device_ids.append(row)
            elif dups.get(device_id):
                dups[device_id].append(row)
            elif old := device_ids.get(device_id):
                dups[device_id] += [old, row]
                device_ids.pop(device_id)
            else:
                device_ids[device_id] = row

            count = idx + 1

    device_ids_count = len(device_ids.keys())
    non_device_ids_count = len(non_device_ids)
    dups_count = sum([len(dups[i]) for i in dups.keys()])

    print(
        f"""
        {_cs(count)} : read row count
        {_cs(device_ids_count + non_device_ids_count + dups_count)}\t: (device_ids + non_device_ids + dups_device_id_row) count
        {_cs(device_ids_count)} : device_ids count
        {_cs(non_device_ids_count)} : non_device_ids count
        {_cs(dups_count)} : dups_device_id_row count
        {_cs(len(dups.keys()))} : dups_device_id count
    """
    )

    with open("./gmes_device_ids.csv", "w") as f:
        writer = csv.DictWriter(f, fieldnames=gmes_columns)

        writer.writeheader()
        writer.writerows(device_ids.values())

    with open("./gmes_non_device_ids.csv", "w") as f:
        writer = csv.DictWriter(f, fieldnames=gmes_columns)

        writer.writeheader()
        writer.writerows(non_device_ids)

    with open("./gmes_duped_device_ids.csv", "w") as f:
        f.writelines([json.dumps(j) + "\n" for j in dups.values()])


def gmes_mac_deduped():
    macs = {}
    dups = defaultdict(list)
    with open("./gmes_non_device_ids.csv", "r") as f:
        reader = csv.DictReader(f)

        count = 0
        skipped = 0
        for idx, row in enumerate(reader):
            mac = row.get("macAddress")

            if not mac:
                skipped += 1
                continue
            elif dups.get(mac):
                dups[mac].append(row)
            elif old := macs.get(mac):
                dups[mac] += [old, row]
                macs.pop(mac)
            else:
                macs[mac] = row

            count = idx + 1

    macs_count = len(macs.keys())
    dups_count = sum([len(dups[i]) for i in dups.keys()])

    print(
        f"""
        {count}\t: read row count
        {macs_count + dups_count + skipped}\t: (macs + dups_mac_rows + skipped) count
        {macs_count}\t: macs count
        {dups_count}\t: dups_mac_rows count
        {skipped}\t: skipped count
        {len(dups.keys())}\t: dups_mac count
    """
    )

    with open("./gmes_mac.csv", "w") as f:
        writer = csv.DictWriter(f, fieldnames=gmes_columns)

        writer.writeheader()
        writer.writerows(macs.values())

    with open("./gmes_duped_mac.csv", "w") as f:
        f.writelines([json.dumps(j) + "\n" for j in dups.values()])


# make_refined_gmes()
# seperate_by_device_id()
# gmes_mac_deduped()


make_refined_productmodel()
