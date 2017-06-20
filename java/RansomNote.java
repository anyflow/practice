import java.util.*;

class RansomNote {
  static Map<String, Integer> get(String[] words) {
    Map<String, Integer> ret = new HashMap<>();
    for (String word : words) {
      Integer count = ret.get(word);
      ret.put(word, count == null ? 1 : ++count);
    }

    return ret;
  }

  static boolean isAvailable(Map<String, Integer> dic, Map<String, Integer> target) {
    if (target.size() <= 0) {
      return true;
    }

    if (dic == null || dic.size() <= 0) {
      return false;
    }

    for (String word : target.keySet()) {
      Integer count = target.get(word);
      Integer countInDic = dic.get(word);
      if (countInDic == null || count > countInDic) {
        return false;
      }
    }
    return true;
  }

  public static void main(String[] args) {
    String dics = "wi z ne we ebixk yvrd qrd ojckw q xe e bcco xb ieqym dwuu w dnapw achkt xqdhs nstms zmqu csqxi rim tvic arq fvjqx x su ty zl zmah y tv rkjq hpvpx ujj f i u fl iv n mnrvx tho diz k tidi gr ptkq z tho su diz yvrd dwuu dnapw xb arq xb mnrvx xe bcco qrd y ptkq rim fvjqx bcco q q wi i tidi gr mnrvx hpvpx tv f y mnrvx we fvjqx tv f wi ptkq ujj rim ebixk tho ptkq rkjq yvrd dwuu zl ujj zl qrd e ieqym";
    String words = "dwuu tvic y dnapw ujj tidi nstms x xe achkt x su zmqu iv zmqu xb ojckw we fvjqx tvic tv ne rkjq diz tvic we rkjq nstms zmah ieqym zmah fl xb wi tho x z ty u i gr ptkq q su tho rim tv iv iv yvrd xe qrd y dnapw q zmah arq we ieqym su zl q xb arq rkjq q e xb zl ty fvjqx ptkq ieqym qrd y wi wi nstms diz dnapw zmah q ebixk su e xb q i mnrvx wi x x tidi w ojckw bcco e tv rkjq tho";

    String[] magazine = dics.split(" ");
    String[] ransom = words.split(" ");

    Map<String, Integer> magazineSet = get(magazine);
    Map<String, Integer> ransomSet = get(ransom);

    System.out.println(isAvailable(magazineSet, ransomSet) ? "Yes" : "No");
  }
}