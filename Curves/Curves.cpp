/**********************************************************************************
// Curves (Código Fonte)
//
// Criação:     12 Ago 2020
// Atualização: 06 Ago 2023
// Compilador:  Visual C++ 2022
//
// Aluno: Níckolas Emanuel de Oliveira Silva
// Descrição:   TRABALHO PRÁTICO I
**********************************************************************************/

#include "DXUT.h"
#include "cmath"

// ------------------------------------------------------------------------------

struct Vertex
{
    XMFLOAT3 Pos;
    XMFLOAT4 Color;
};

// ------------------------------------------------------------------------------


class Curves : public App
{
private:
    ID3D12RootSignature* rootSignature;
    ID3D12PipelineState* pipelineState;

    //CONSTANTES
    static const uint MaxVertex = 4;
    static const uint limCurvas = 20;
    static const uint flag = 100;
    static const uint vertexSize = sizeof(Vertex);

    Mesh* lcMesh;
    Mesh* curvasMesh;
    Mesh* iconsMesh;
    Mesh* iconOnVertexMesh;

    uint coutCurvas = 0;
    uint curvaIndex = 0;
    uint count = 0;
    uint index = 0;

    Vertex pontos_de_controle[MaxVertex];
    Vertex curvas[flag];
    Vertex flagCurvas[limCurvas * flag];
    Vertex icons[3];
    Vertex iconOnVertex[MaxVertex][3];

    //SALVAR
    Vertex pontos_salvar[MaxVertex];
    Vertex curvas_salvar[flag];
    Vertex flag_salvar[limCurvas * flag];
    Vertex iconOnVertex_salvar[MaxVertex][3];

    uint count_salvar = 0;
    uint countCurvas_salvar = 0;
    uint index_salvar = 0;
    uint indexCurva_salvar = 0;

public:
    void Init();
    void Update();
    void Display();
    void Finalize();

    void BuildRootSignature();
    void BuildPipelineState();

    //---------------------ADICIONADOS---------------------
    float fazerCurva(float, float, float, float, float);
    void salvar();
    void carregar();
    void limpar();
};

//---------------------SALVAR---------------------
void Curves::salvar() {

    //uint
    indexCurva_salvar = curvaIndex;
    index_salvar = index;
    countCurvas_salvar = coutCurvas;
    count_salvar = count;

    //Vertex
    //pontos de controle
    for (int x = 0; x < index; x++) {
        pontos_salvar[x] = pontos_de_controle[x];
    }

    //curvas
    for (int x = 0; x < flag; x++) {
        curvas_salvar[x] = curvas[x];
    }
    for (int x = 0; x < (curvaIndex * 100) + 100; x++) {
        flag_salvar[x] = flagCurvas[x];
    }

    //icones no vertex
    for (int i = 0; i < MaxVertex; ++i) {
        for (int j = 0; j < 3; ++j) {
            iconOnVertex_salvar[i][j] = iconOnVertex[i][j];
        }
    }
}

//---------------------CARREGAR---------------------
void Curves::carregar() {
    //uint
    index = index_salvar;
    count = count_salvar;
    coutCurvas = countCurvas_salvar;
    curvaIndex = indexCurva_salvar;

    //Vertex
    //pontos de controle
    for (int x = 0; x < index; x++) {
        pontos_de_controle[x] = pontos_salvar[x];
    }
    //curvas
    for (int x = 0; x < flag; x++) {
        curvas[x] = curvas_salvar[x];
    }
    for (int x = 0; x < (curvaIndex * 100) + 100; x++) {
        flagCurvas[x] = flag_salvar[x];
    }
    //icones do vertex
    for (int i = 0; i < MaxVertex; ++i) {
        for (int j = 0; j < 3; ++j) {
            iconOnVertex[i][j] = iconOnVertex_salvar[i][j];
        }
    }
}

//---------------------LIMPAR---------------------
void Curves::limpar() {
    //curvas
    curvaIndex = 0;
    coutCurvas = 0;
    std::memset(curvas, 0, sizeof(curvas));
    std::memset(flagCurvas, 0, sizeof(flagCurvas));

    //pontos de controle
    index = 0;
    count = 0;
    std::memset(pontos_de_controle, 0, vertexSize);

    //icones
    std::memset(icons, 0, sizeof(icons));
    std::memset(iconOnVertex, 0, sizeof(iconOnVertex));

}

//---------------------CALCULO CURVA DE BERNSTEIN---------------------
float Curves::fazerCurva(float t, float p1, float p2, float p3, float p4) {
    float result;

    //result = ((1 - t) * (1 - t) * (1 - t) * p1 + (3 * t) * (1 - t) * (1 - t)
        //* p2 + 3 * (t * t) * (1 - t) * p3 + (t * t * t) * p4);

    result = (pow((1 - t), 3) * p1 + (3 * t) * pow((1 - t), 2) * p2 + 3 * (t * t) * (1 - t) * p3 + pow(t, 3) * p4);

    return result;
}

void Curves::Init()
{
    graphics->ResetCommands();

    // ---------[ Build Geometry ]------------
    
    // tamanho do buffer de vértices em bytes
    const uint vbSize = MaxVertex * sizeof(Vertex);

    // cria malha 3D
    lcMesh = new Mesh(vbSize, vertexSize);
    iconsMesh = new Mesh((3 * vertexSize), vertexSize);
    curvasMesh = new Mesh((limCurvas * vertexSize * flag), vertexSize);
    iconOnVertexMesh = new Mesh((3 * vertexSize * limCurvas), vertexSize);

    BuildRootSignature();
    BuildPipelineState();        
    graphics->SubmitCommands();
}

// ------------------------------------------------------------------------------

void Curves::Update()
{
    float cx = float(window->CenterX());
    float cy = float(window->CenterY());
    float mx = float(input->MouseX());
    float my = float(input->MouseY());

    // converte as coordenadas da tela para a faixa -1.0 a 1.0
    // cy e my foram invertidos para levar em consideração que 
    // o eixo y da tela cresce na direção oposta do cartesiano
    float x = (mx - cx) / cx;
    float y = (cy - my) / cy;

    pontos_de_controle[index] = { XMFLOAT3(x, y, 0.0f), XMFLOAT4(Colors::White) };

    //MONTANDO O ICONE DE TRIANGULO
    const float size = 0.04f;  
    const float height = sqrt(3.0f) / 2.0f * size;

    icons[0] = { XMFLOAT3(x, y + height / 2.0f, 0.0f), XMFLOAT4(Colors::Yellow) }; // Ponto superior
    icons[1] = { XMFLOAT3(x - size / 2.0f, y - height / 2.0f, 0.0f), XMFLOAT4(Colors::Yellow) }; // Ponto inferior esquerdo
    icons[2] = { XMFLOAT3(x + size / 2.0f, y - height / 2.0f, 0.0f), XMFLOAT4(Colors::Yellow) }; // Ponto inferior direito

    //Faz uma curva se tiver 4 pontos de controle definidos
    if (index == 3) {
        float t;
        XMFLOAT3 pos;

        // Calcula as curvas
        for (int i = 0; i < flag; ++i) {
            t = float(i) / float(flag - 1);

            pos = XMFLOAT3(
                fazerCurva(t, pontos_de_controle[0].Pos.x, pontos_de_controle[1].Pos.x, pontos_de_controle[2].Pos.x, pontos_de_controle[3].Pos.x),
                fazerCurva(t, pontos_de_controle[0].Pos.y, pontos_de_controle[1].Pos.y, pontos_de_controle[2].Pos.y, pontos_de_controle[3].Pos.y),
                0.0f
            );

            curvas[i] = { pos, XMFLOAT4(Colors::Red) };
        }

        int startIdx = curvaIndex * 100;
        for (int i = startIdx; i < startIdx + 100; ++i) {
            flagCurvas[i] = curvas[i % 100];
        }
    }
    //-------------------OPÇÕES DE INPUT-------------------
 
    // sai com o pressionamento da tecla ESC
    if (input->KeyPress(VK_ESCAPE))
        window->Close();

    // salva as linhas pressionando a tecla 's' ou 'S'
    if (input->KeyPress('s') || input->KeyPress('S')) { salvar(); }

    // carrega as linhas salvas pressionando a tecla 'l' ou 'L'
    if (input->KeyPress('l') || input->KeyPress('L')) { carregar(); }

    // limpa o conteúdo da tela pressionando a tecla 'del'
    if (input->KeyPress(VK_DELETE)) { limpar(); }
    
    // cria vértices com o botão do mouse
    if (input->KeyPress(VK_LBUTTON)) {

        iconOnVertex[index][0] = {XMFLOAT3(x, y + height / 2.0f, 0.0f), XMFLOAT4(Colors::Yellow)}; // Ponto superior
        iconOnVertex[index][1] = {XMFLOAT3(x - size / 2.0f, y - height / 2.0f, 0.0f), XMFLOAT4(Colors::Yellow)}; // Ponto inferior esquerdo
        iconOnVertex[index][2] = { XMFLOAT3(x + size / 2.0f, y - height / 2.0f, 0.0f), XMFLOAT4(Colors::Yellow) }; // Ponto inferior direito

        if (index == 3) {
            index = 2;
            count = 2;

            if (curvaIndex == 19) {                                              
                curvaIndex = 0;
            }
            else {
                curvaIndex++;
                if (curvaIndex < limCurvas)
                    coutCurvas++;
            }
            pontos_de_controle[0].Pos.x = pontos_de_controle[3].Pos.x;
            pontos_de_controle[0].Pos.y = pontos_de_controle[3].Pos.y;
            
            iconOnVertex[0][0] = iconOnVertex[3][0];
            iconOnVertex[0][1] = iconOnVertex[3][1];
            iconOnVertex[0][2] = iconOnVertex[3][2];

            float refX = (pontos_de_controle[3].Pos.x - pontos_de_controle[2].Pos.x) + pontos_de_controle[3].Pos.x;
            float refY = (pontos_de_controle[3].Pos.y - pontos_de_controle[2].Pos.y) + pontos_de_controle[3].Pos.y;

            Vertex ref = {
                XMFLOAT3(refX,refY,0.0f),
                XMFLOAT4(Colors::DarkMagenta)
            };

            pontos_de_controle[1] = ref;

            iconOnVertex[1][0] = { XMFLOAT3(refX - 0.01f, refY - 0.02f, 0.0f), XMFLOAT4(Colors::Yellow) };
            iconOnVertex[1][1] = { XMFLOAT3(refX - 0.01f, refY + 0.02f, 0.0f), XMFLOAT4(Colors::Yellow) };
            iconOnVertex[1][2] = { XMFLOAT3(refX + 0.01f, refY + 0.02f, 0.0f), XMFLOAT4(Colors::Yellow) };
        }
        else {
            index++;
            count++;
        }
    }
    
    graphics->ResetCommands();
    graphics->Copy(pontos_de_controle, lcMesh->vertexBufferSize, lcMesh->vertexBufferUpload, lcMesh->vertexBufferGPU);
    graphics->Copy(icons, iconsMesh->vertexBufferSize, iconsMesh->vertexBufferUpload, iconsMesh->vertexBufferGPU);
    graphics->Copy(iconOnVertex, iconOnVertexMesh->vertexBufferSize, iconOnVertexMesh->vertexBufferUpload, iconOnVertexMesh->vertexBufferGPU);
    graphics->Copy(flagCurvas, curvasMesh->vertexBufferSize, curvasMesh->vertexBufferUpload, curvasMesh->vertexBufferGPU);
    graphics->SubmitCommands();
    Display();
}

// ------------------------------------------------------------------------------

void Curves::Display()
{
    // limpa backbuffer
    graphics->Clear(pipelineState);

    // submete comandos de configuração do pipeline
    graphics->CommandList()->SetGraphicsRootSignature(rootSignature);
    graphics->CommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINESTRIP);
    graphics->CommandList()->IASetVertexBuffers(0, 1, lcMesh->VertexBufferView());

    // submete comandos de desenho
    graphics->CommandList()->DrawInstanced(min(count + 1, 4), 1, 0, 0);

    //Icons no Vertex
    graphics->CommandList()->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    graphics->CommandList()->IASetVertexBuffers(0, 1, iconOnVertexMesh->VertexBufferView());
    graphics->CommandList()->DrawInstanced(MaxVertex * 3, 1, 0, 0);

    //ICONS
    //graphics->CommandList()->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    graphics->CommandList()->IASetVertexBuffers(0, 1, iconsMesh->VertexBufferView());
    graphics->CommandList()->DrawInstanced(5, 1, 0, 0);

    bool allLine = count == 3;
    //CURVA
    graphics->CommandList()->IASetVertexBuffers(0, 1, curvasMesh->VertexBufferView());
    graphics->CommandList()->DrawInstanced(flag * (allLine ? coutCurvas + 1 : coutCurvas), 1, 0, 0);
    // apresenta backbuffer
    graphics->Present();    
}

// ------------------------------------------------------------------------------

void Curves::Finalize()
{
    rootSignature->Release();
    pipelineState->Release();
    delete lcMesh;
    delete iconsMesh;
    delete iconOnVertexMesh;
    delete curvasMesh;
}


// ------------------------------------------------------------------------------
//                                     D3D                                      
// ------------------------------------------------------------------------------

void Curves::BuildRootSignature()
{
    // descrição para uma assinatura vazia
    D3D12_ROOT_SIGNATURE_DESC rootSigDesc = {};
    rootSigDesc.NumParameters = 0;
    rootSigDesc.pParameters = nullptr;
    rootSigDesc.NumStaticSamplers = 0;
    rootSigDesc.pStaticSamplers = nullptr;
    rootSigDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

    // serializa assinatura raiz
    ID3DBlob* serializedRootSig = nullptr;
    ID3DBlob* error = nullptr;

    ThrowIfFailed(D3D12SerializeRootSignature(
        &rootSigDesc,
        D3D_ROOT_SIGNATURE_VERSION_1,
        &serializedRootSig,
        &error));

    // cria uma assinatura raiz vazia
    ThrowIfFailed(graphics->Device()->CreateRootSignature(
        0,
        serializedRootSig->GetBufferPointer(),
        serializedRootSig->GetBufferSize(),
        IID_PPV_ARGS(&rootSignature)));
}

// ------------------------------------------------------------------------------

void Curves::BuildPipelineState()
{
    // --------------------
    // --- Input Layout ---
    // --------------------
    
    D3D12_INPUT_ELEMENT_DESC inputLayout[2] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
    };

    // --------------------
    // ----- Shaders ------
    // --------------------

    ID3DBlob* vertexShader;
    ID3DBlob* pixelShader;

    D3DReadFileToBlob(L"Shaders/Vertex.cso", &vertexShader);
    D3DReadFileToBlob(L"Shaders/Pixel.cso", &pixelShader);

    // --------------------
    // ---- Rasterizer ----
    // --------------------

    D3D12_RASTERIZER_DESC rasterizer = {};
    rasterizer.FillMode = D3D12_FILL_MODE_WIREFRAME;
    rasterizer.CullMode = D3D12_CULL_MODE_NONE;
    rasterizer.FrontCounterClockwise = FALSE;
    rasterizer.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
    rasterizer.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
    rasterizer.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
    rasterizer.DepthClipEnable = TRUE;
    rasterizer.MultisampleEnable = FALSE;
    rasterizer.AntialiasedLineEnable = FALSE;
    rasterizer.ForcedSampleCount = 0;
    rasterizer.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

    // ---------------------
    // --- Color Blender ---
    // ---------------------

    D3D12_BLEND_DESC blender = {};
    blender.AlphaToCoverageEnable = FALSE;
    blender.IndependentBlendEnable = FALSE;
    const D3D12_RENDER_TARGET_BLEND_DESC defaultRenderTargetBlendDesc =
    {
        FALSE,FALSE,
        D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
        D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
        D3D12_LOGIC_OP_NOOP,
        D3D12_COLOR_WRITE_ENABLE_ALL,
    };
    for (UINT i = 0; i < D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT; ++i)
        blender.RenderTarget[i] = defaultRenderTargetBlendDesc;

    // ---------------------
    // --- Depth Stencil ---
    // ---------------------

    D3D12_DEPTH_STENCIL_DESC depthStencil = {};
    depthStencil.DepthEnable = TRUE;
    depthStencil.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
    depthStencil.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
    depthStencil.StencilEnable = FALSE;
    depthStencil.StencilReadMask = D3D12_DEFAULT_STENCIL_READ_MASK;
    depthStencil.StencilWriteMask = D3D12_DEFAULT_STENCIL_WRITE_MASK;
    const D3D12_DEPTH_STENCILOP_DESC defaultStencilOp =
    { D3D12_STENCIL_OP_KEEP, D3D12_STENCIL_OP_KEEP, D3D12_STENCIL_OP_KEEP, D3D12_COMPARISON_FUNC_ALWAYS };
    depthStencil.FrontFace = defaultStencilOp;
    depthStencil.BackFace = defaultStencilOp;
    
    // -----------------------------------
    // --- Pipeline State Object (PSO) ---
    // -----------------------------------

    D3D12_GRAPHICS_PIPELINE_STATE_DESC pso = {};
    pso.pRootSignature = rootSignature;
    pso.VS = { reinterpret_cast<BYTE*>(vertexShader->GetBufferPointer()), vertexShader->GetBufferSize() };
    pso.PS = { reinterpret_cast<BYTE*>(pixelShader->GetBufferPointer()), pixelShader->GetBufferSize() };
    pso.BlendState = blender;
    pso.SampleMask = UINT_MAX;
    pso.RasterizerState = rasterizer;
    pso.DepthStencilState = depthStencil;
    pso.InputLayout = { inputLayout, 2 };
    pso.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
    pso.NumRenderTargets = 1;
    pso.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
    pso.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
    pso.SampleDesc.Count = graphics->Antialiasing();
    pso.SampleDesc.Quality = graphics->Quality();
    graphics->Device()->CreateGraphicsPipelineState(&pso, IID_PPV_ARGS(&pipelineState));

    vertexShader->Release();
    pixelShader->Release();
}

// ------------------------------------------------------------------------------
//                                  WinMain                                      
// ------------------------------------------------------------------------------

int APIENTRY WinMain(_In_ HINSTANCE hInstance,    _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
    try
    {
        // cria motor e configura a janela
        Engine* engine = new Engine();
        engine->window->Mode(WINDOWED);
        engine->window->Size(1024, 600);
        engine->window->ResizeMode(ASPECTRATIO);
        engine->window->Color(0, 0, 0);
        engine->window->Title("Curves");
        engine->window->Icon(IDI_ICON);
        engine->window->LostFocus(Engine::Pause);
        engine->window->InFocus(Engine::Resume);

        // cria e executa a aplicação
        engine->Start(new Curves());

        // finaliza execução
        delete engine;
    }
    catch (Error & e)
    {
        // exibe mensagem em caso de erro
        MessageBox(nullptr, e.ToString().data(), "Curves", MB_OK);
    }

    return 0;
}

// ----------------------------------------------------------------------------
