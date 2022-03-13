import json
import os
import csv

os.chdir("./python/sales_model_name_compare")


def percent(target):
    return "{:,.2f}".format(target * 100)


def _cs(target):
    return "{:,}".format(target)


def sales_model_name(target):
    if not isinstance(target, str):
        return False

    tokens = target.split(".")

    if len(tokens) != 2:
        return False

    if len(tokens[0]) < 1 or len(tokens[0]) > 11:
        return False

    if len(tokens[1]) < 1 or len(tokens[1]) > 7:
        return False

    return True


def device_id_map():
    ret = {}
    global valid_salesmodel_count
    valid_salesmodel_count = 0
    with open("./gmes_device_ids.csv", "r") as f:
        reader = csv.DictReader(f)

        def add(item, target):
            target[item["deviceId"]] = item

            if sales_model_name(item["salesModel"]):
                global valid_salesmodel_count
                valid_salesmodel_count += 1

        [add(row, ret) for row in reader]

    print(
        f"{_cs(valid_salesmodel_count)} / {_cs(len(ret.keys()))} ({percent(valid_salesmodel_count/len(ret.keys())) } %)\t: valid / total(device_ids)"
    )
    return ret


def duped_device_id_map():
    ret = {}
    valid_salesmodel_count = 0

    with open("./gmes_duped_device_ids.csv", "r") as f:
        for row in f.readlines():
            item = json.loads(row)
            ret[item[0]["deviceId"]] = item[0]

            if sales_model_name(item[0]["salesModel"]):
                valid_salesmodel_count += 1

    print(
        f"{_cs(valid_salesmodel_count)} / {_cs(len(ret.keys()))} ({percent(valid_salesmodel_count/len(ret.keys())) } %)\t: valid / total(duped device_ids)"
    )
    return ret


def mac_map():
    ret = {}
    global valid_salesmodel_count
    valid_salesmodel_count = 0

    with open("./gmes_mac.csv", "r") as f:
        reader = csv.DictReader(f)

        def add(item, target):
            target[item["macAddress"]] = item

            if sales_model_name(item["salesModel"]):
                global valid_salesmodel_count
                valid_salesmodel_count += 1

        [add(row, ret) for row in reader]

    print(
        f"{_cs(valid_salesmodel_count)} / {_cs(len(ret.keys()))} ({percent(valid_salesmodel_count/len(ret.keys())) } %)\t: valid / total(macs)"
    )
    return ret


def duped_mac_map():
    ret = {}
    valid_salesmodel_count = 0

    with open("./gmes_duped_mac.csv", "r") as f:
        for row in f.readlines():
            item = json.loads(row)
            ret[item[0]["macAddress"]] = item[0]

            if sales_model_name(item[0]["salesModel"]):
                valid_salesmodel_count += 1

    print(
        f"{_cs(valid_salesmodel_count)} / {_cs(len(ret.keys()))} ({percent(valid_salesmodel_count/len(ret.keys())) } %)\t: valid / total(duped macs)"
    )
    return ret


def stat(input_type=None):
    (device_ids, macs, duped_device_ids, duped_macs) = (
        device_id_map(),
        mac_map(),
        duped_device_id_map(),
        duped_mac_map(),
    )

    total = 0
    device_id_count = 0
    mac_count = 0
    duped_device_id_count = 0
    duped_mac_count = 0
    skipped = []

    def fix_nulls(s):
        for line in s:
            yield line.replace("\0", " ")

    with open("./productmodel-eic.csv", "rt") as f:
        reader = csv.DictReader(fix_nulls(f), delimiter="\t")

        read_count = 0
        for row in reader:
            if input_type and row["writeBy"] != input_type:
                continue

            read_count += 1
            (device_id, mac) = (row["deviceId"], row["macAddress"])

            if device_ids.get(device_id):
                total += 1
                device_id_count += 1
            elif macs.get(mac):
                total += 1
                mac_count += 1
            elif duped_device_ids.get(device_id):
                total += 1
                duped_device_id_count += 1
            elif duped_macs.get(mac):
                total += 1
                duped_mac_count += 1
            else:
                skipped.append(row)

    with open("./no_matching.csv", "w") as f:
        writer = csv.DictWriter(
            f,
            fieldnames=[
                "deviceId",
                "macAddress",
                "serialNumber",
                "salesModelName",
                "writeBy",
            ],
        )

        writer.writeheader()
        writer.writerows(skipped)

    found = device_id_count + mac_count

    return f"""
        {_cs(read_count)}\t({percent(read_count/read_count)} %)\t: read count
        {_cs(found)}\t({percent(found/read_count)} %)\t: found (by deviceId + mac)
        {_cs(total)}\t({percent(total/read_count)} %)\t: found w/ duped(found in duped deviceId + mac)
        {_cs(len(skipped))}\t({percent(len(skipped)/read_count)} %)\t: skipped
        {_cs(device_id_count)}\t({percent(device_id_count/read_count)} %)\t: deviceId found
        {_cs(mac_count)}\t({percent(mac_count/read_count)} %)\t: MAC found
        {_cs(duped_device_id_count)}\t({percent(duped_device_id_count/read_count)} %)\t: (duped) deviceId found
        {_cs(duped_mac_count)}\t({percent(duped_mac_count/read_count)} %)\t: (duped) MAC found
    """


print(stat("Q"))
