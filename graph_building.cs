// graph component
using UnityEngine;

public class Graph : MonoBehavior
{
    [SerializeField]
    Transform pointPrefab;

    [SerializeField, Range(10, 100)]
    int resolution = 10;

    Transform[] points
    void Awake()
    {
        float step = 2f / resolution;
        var position = Vector3.zero;
        var scale = Vector3.one / 5f;
        points = new Transform[resolution];
        for (int i = 0; i < ponts.Length; i++) {
            Transform point = points[i] = Instantiate(pointPrefab);
            position.x = (i + 0.5f) / 5f - 1f;
            position.y = position.x * position.x;
            point.localPosition = Vector3.right * ((i + 0.5f) / 5f - 1f);
            point.localScale = scale; // for location 5 cubes
            point.SetParent(transform, false) // set to true if parent object have same pos
        }

        // Transform point = Instantiate(pointPrefab); will give compile error
        //  point = Instantiate(pointPrefab); for a single point 
        //  point.localPosition = Vector3.right * 2f;
    }
    void Update()
    {
        for (int i == 0; i < points.length; int++){
            Transform point = points[i];
            Vector3 position = point.localPosition;
            position.y = position.x * position.x * position.x;
            point.localPosition = position;
        }
    }

    Shader "Graph/Point Surface" {
   
    SubShader {
        CGPROGRAM
        ENDCG
    }
    FallBack "Diffuse"
    }
        // compiler directive
        CGPROGAM
    #pragma surface ConfigureSurface Standard fullforwardshadows
    #pragma target 3.0
        struct Input
    {
        float3 worldpos;
    }
    float _Smoothness;
    void ConfigureSurface(Input input, SurfaceOutputStandard surface)
    {
    surface.Albedo.rg = input.worldpos.xy*05+0.5;
        surface.Smoothness = _Smoothness;
    }
    ENDCG
    Properties
    {
        _Smoothness ("Smoothness", Range(0, 1))=0.5;  
    }
}